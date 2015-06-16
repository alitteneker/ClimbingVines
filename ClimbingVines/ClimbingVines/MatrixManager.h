
#ifndef _TreeTest_MatrixManager_
#define _TreeTest_MatrixManager_

#include "matm.h"
#include <assert.h>

// Matrix stack that can be used to push and pop the modelview matrix.
class MatrixStack {
    int    _index;
    int    _size;
    mat4*  _matrices;
    
public:
    MatrixStack( int numMatrices = 512 ):_index(0), _size(numMatrices)
    { _matrices = new mat4[numMatrices]; }
    
    ~MatrixStack()
    { delete[]_matrices; }
    
    void push( const mat4& m ) {
        assert( _index + 1 < _size );
        _matrices[_index++] = m;
    }
    
    void empty() {
        _index = 0;
    }
    
    mat4& pop( void ) {
        assert( _index - 1 >= 0 );
        --_index;
        return _matrices[_index];
    }
};

extern MatrixStack mstack;
extern mat4 mModel;
extern mat4 mView;

#endif
