/*
 * Face.h
 *
 *  Created on: Sep 9, 2015
 *      Author: will
 */

#ifndef _FACE_H_
#define _FACE_H_

#include "Vec.h"


class Face {
public:
		int v[3];

		Face() {label = -999;}
		Face(const int &v0, const int &v1, const int &v2)
			{ v[0] = v0; v[1] = v1; v[2] = v2; label = -999;}
		Face(const int &v0, const int &v1, const int &v2, const int &vt0, const int &vt1, const int &vt2)
		{ v[0] = v0; v[1] = v1; v[2] = v2; vt[0] = vt0; vt[1] = vt1; vt[2] = vt2; label = -999;}
		Face(const int *v_)
			{ v[0] = v_[0]; v[1] = v_[1]; v[2] = v_[2]; label = -999; }
		int &operator[] (int i) { return v[i]; }
		const int &operator[] (int i) const { return v[i]; }
		operator const int * () const { return &(v[0]); }
		operator const int * () { return &(v[0]); }
		operator int * () { return &(v[0]); }
		int indexof(int v_) const
		{
			return (v[0] == v_) ? 0 :
			       (v[1] == v_) ? 1 :
			       (v[2] == v_) ? 2 : -1;
		}
		vec facenormal;
		float faceArea;
		point faceCenter;
    float intensity;
    float color[3];
    int label;
    // vts are saved as indices to mesh.vts DS
    // So they're -1 from their file value b/c vts start at 1 in file but
    // they start at 0 in mesh.vts
    int vt[3];
    int index;

    bool equals(Face);
	};



#endif /* LIBS_CPP_TRIMESH2_FACE_H_ */
