#include "life_mpi.hpp"

#include <cstdlib>
#include <mpi.h>

#include "utils.hpp"
#include "dbg.hpp"

const int NEW_FIELD_TAG = 1;
const int STATUS_TAG = 2;
const int REPORT_TAG = 3;
const int RUN_TAG = 4;
const int DETACH_TAG = 5;
const int STOP_TAG = 6;
const int RIGHT_UPDATE_TAG = 7;
const int LEFT_UPDATE_TAG = 8;
const int DONE_TAG = 9;

using namespace utility;

LifeGameMpiMaster::LifeGameMpiMaster(const field_t &field)
: field(field), width(field.size()), height(field[0].size()),
  running(false), maxVersion(0)
{
  MPI_Comm_size(MPI_COMM_WORLD, &nodesCount);
  int chunkSize = field.size() / (nodesCount - 1);
  if (chunkSize != 0) {
    bufSize = packSize(2 * chunkSize, height) + 1000;
  } else {
    bufSize = packSize(width, height) + 1000;
  }
  buf = (char*)malloc(bufSize);
  int curChunk = 0;
  for (size_t i = 1; i < nodesCount; ++i) {
    //(currentChunk, currentChunk + chunkSize)
    size_t messageLen;
    pack(field, curChunk - 1,
      1 + (i == nodesCount - 1 ? width : curChunk + chunkSize),
      buf, messageLen);
      DBG(curChunk - 1 << " " << 1 + (i == nodesCount - 1 ? width : curChunk + chunkSize));
      DBG("Sending field to " << i);
      DBG(toString(unpack(buf)));
    MPI_Send(buf, messageLen, MPI_CHAR, i, NEW_FIELD_TAG, MPI_COMM_WORLD);
    curChunk += chunkSize;
  }

}

void LifeGameMpiMaster::run(size_t stepCount) {
  int buf[1];
  buf[0] = stepCount;
  for (size_t i = 1; i < nodesCount; ++i) {
    MPI_Send(buf, 1, MPI_INT, i, RUN_TAG, MPI_COMM_WORLD);
    DBG("Sending run to " << i);
  }
  maxVersion += stepCount;
}

void LifeGameMpiMaster::runwait(size_t stepCount) {
  int buf[1];
  maxVersion += stepCount;
  buf[0] = stepCount;
  for (size_t i = 1; i < nodesCount; ++i) {
    MPI_Send(buf, 1, MPI_INT, i, RUN_TAG, MPI_COMM_WORLD);
    DBG("Sending run to " << i);
  }
  MPI_Status status;
  for (size_t i = 1; i < nodesCount; ++i) {
    int versionRecieved = 0;
    while (versionRecieved < maxVersion) {
      MPI_Recv(&versionRecieved, 1, MPI_INT, i, DONE_TAG, MPI_COMM_WORLD, &status);
    }
  }
}

void LifeGameMpiMaster::stop() {
  for (size_t i = 1; i < nodesCount; ++i) {
    MPI_Send(buf, 0, MPI_CHAR, i, STOP_TAG, MPI_COMM_WORLD);
    DBG("Sending stop to " << i);
  }
  for (size_t i = 1; i < nodesCount; ++i) {
    MPI_Send(buf, 0, MPI_CHAR, i, STATUS_TAG, MPI_COMM_WORLD);
    DBG("Sending status to " << i);
  }
  std::vector<size_t> versions;
  int maxVersion = 0;
  for (size_t i = 1; i < nodesCount; ++i) {
    MPI_Status status;
    MPI_Recv(buf, bufSize, MPI_CHAR, i, REPORT_TAG, MPI_COMM_WORLD, &status);
    size_t version = ((size_t*)buf)[0];
    versions.push_back(version);
    if (version >= maxVersion) {
      maxVersion = version;
    }
    DBG("Recieved report from " << i << " version " << version);
  }
  this->maxVersion = maxVersion;
  int buf[1];
  for (size_t i = 1; i < nodesCount; ++i) {
    buf[0] = maxVersion - versions[i - 1];
    MPI_Send(buf, 1, MPI_INT, i, RUN_TAG, MPI_COMM_WORLD);
    DBG("Sending run to " << i);
  }
}

void LifeGameMpiMaster::sync() {
  for (size_t i = 1; i < nodesCount; ++i) {
    MPI_Send(buf, 0, MPI_CHAR, i, STATUS_TAG, MPI_COMM_WORLD);
    DBG("Sending status to " << i);

  }
  field.clear();
  for (size_t i = 1; i < nodesCount; ++i) {
    MPI_Status status;
    MPI_Recv(buf, bufSize, MPI_CHAR, i, REPORT_TAG, MPI_COMM_WORLD, &status);
    size_t version = ((size_t*)buf)[0];
    field_t part = unpack(buf + sizeof(size_t));
    DBG("Recieved report from " << i << " version " << version);
    field.insert(field.end(), part.begin() + 1, part.end() - 1);
  }
}

//============= Life Game Mpi Slave =========================

LifeGameMpiSlave::LifeGameMpiSlave() {
  MPI_Comm_size(MPI_COMM_WORLD, &nodesCount);
  bufSize = 0;
  buf = (char*) malloc(bufSize);
  running = false;
  maxVersion = version = 0;
  detached = false;
}

void LifeGameMpiSlave::run() {
  while (!detached) {
    while (running && version < maxVersion) {
      field[0] = leftBorderQueue.front();
      field[width - 1] = rightBorderQueue.front();
      leftBorderQueue.pop();
      rightBorderQueue.pop();


      field_t cpy = field;
      for (size_t i = 1; i < field.size() - 1; ++i) {
        for (size_t j = 0; j < field[i].size(); ++j) {
          int height = field[i].size();
          int numAlive = 0;
          for (int di = -1; di <= 1; ++di) {
            for (int dj = -1; dj <= 1; ++dj) {
              int nj = j + dj;
              if (nj < 0)
                nj += height;
              if (nj >= height)
                nj -= height;
              numAlive += field[i + di][nj];
            }
          }
          numAlive -= field[i][j];
          if (numAlive == 3 || (numAlive == 2 && field[i][j]))
            cpy[i][j] = true;
          else
            cpy[i][j] = false;
        }
      }
      int rank;
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
      DBG("node " << rank << "field was " << std::endl << toString(field)
          << " now " << std::endl << toString(cpy));

      size_t reqLen = packSize(1, height);
      adjustBuffer(2 * reqLen);
      size_t len1, len2;
      pack(cpy, 1, 2, buf, len1);
      pack(cpy, width - 2, width - 1, buf + len1, len2);
      MPI_Request req1, req2;
      MPI_Issend(buf, len1, MPI_CHAR,
                1 + (nodesCount + rank - 3) % (nodesCount - 1),
                LEFT_UPDATE_TAG, MPI_COMM_WORLD, &req1);

      MPI_Isend(buf + len1, len2, MPI_CHAR,
                1 + (rank) % (nodesCount - 1),
                RIGHT_UPDATE_TAG, MPI_COMM_WORLD, &req2);

      while (!running || leftBorderQueue.empty() || rightBorderQueue.empty()) {
        handleMessage();
      }
      MPI_Status status;
      MPI_Wait(&req1, &status);
      MPI_Wait(&req2, &status);
      version++;
      field = cpy;
      if (version >= maxVersion) {
        MPI_Request req;
        MPI_Isend(&maxVersion, 1, MPI_INT, 0, DONE_TAG, MPI_COMM_WORLD, &req);
      }
    }
    handleMessage();

  }
}

void LifeGameMpiSlave::adjustBuffer(MPI_Status& status, MPI_Datatype type) {
  int count;
  MPI_Get_count(&status, type, &count);
  adjustBuffer(count);
}

void LifeGameMpiSlave::adjustBuffer(size_t reqSize) {
  if (bufSize < reqSize) {
    bufSize = 100 + reqSize;
    free(buf);
    buf = (char*) malloc(bufSize);
  }
}

bool LifeGameMpiSlave::handleMessage() {
  int result = 1;
  MPI_Status status;
  MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  if (result) {
    int source, tag, error;
    source = status.MPI_SOURCE;
    tag = status.MPI_TAG;
    error = status.MPI_ERROR;
    if (tag == NEW_FIELD_TAG) {
      adjustBuffer(status, MPI_CHAR);
      MPI_Recv(buf, bufSize, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
      DBG("Recieved new field");
      field = unpack(buf);
      DBG(toString(field));
      width = field.size();
      height = field[0].size();
      leftBorderQueue.push(field[0]);
      rightBorderQueue.push(field[width - 1]);
    } else if (tag == STATUS_TAG) {
      MPI_Recv(buf, bufSize, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
      DBG("Recieved status");
      size_t reqLen = packSize(field.size(), field[0].size());
      adjustBuffer(reqLen);
      size_t len;
      ((size_t*)buf)[0] = version;
      pack(field, buf + sizeof(size_t), len);
      MPI_Send(buf, len + sizeof(size_t), MPI_CHAR, 0, REPORT_TAG, MPI_COMM_WORLD);
      DBG("Reported back");
    } else if (tag == RIGHT_UPDATE_TAG || tag == LEFT_UPDATE_TAG) {
      adjustBuffer(status, MPI_CHAR);
      MPI_Recv(buf, bufSize, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
      DBG("Border update");
      field_t update = unpack(buf);
      if (tag == RIGHT_UPDATE_TAG) {
        leftBorderQueue.push(update[0]);
      } else {
        rightBorderQueue.push(update[0]);
      }
    } else if (tag == STOP_TAG) {
      MPI_Recv(buf, bufSize, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
      DBG("Recieved stop");
      running = false;
    } else if (tag == DETACH_TAG) {
      MPI_Recv(buf, bufSize, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
      DBG("Recieved detach");
      detached = true;
    } else if (tag == RUN_TAG) {
      int stepCountBuf[1];
      MPI_Recv(stepCountBuf, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);

      int stepCount = stepCountBuf[0];
      DBG("Recieved run " << stepCount);
      maxVersion = version + stepCount;
      running = true;
    }
    return true;
  } else {
    return false;
  }
}
