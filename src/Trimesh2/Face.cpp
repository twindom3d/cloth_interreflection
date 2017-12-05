#include "Face.h"

bool Face::equals(Face face)
{
  return (this->v[0] == face[0] || this->v[0] == face[1] || this->v[0] == face[2]) &&
      (this->v[1] == face[0] || this->v[1] == face[1] || this->v[1] == face[2]) &&
      (this->v[2] == face[0] || this->v[2] == face[1] || this->v[2] == face[2]);
}

