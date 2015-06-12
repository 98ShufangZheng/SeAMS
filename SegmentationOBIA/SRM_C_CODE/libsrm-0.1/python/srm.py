import os
import ctypes as C
from ctypes.util import find_library

import numpy
import numpy.ctypeslib as npC

_lib = None
_libname = "srm"
try:
    _lib = npC.load_library("lib" + _libname, os.path.dirname(__file__))
except OSError:
    _fullname = find_library(_libname)
    if not _fullname:
        _fullname = "lib" + _libname + ".so"
    _lib = C.CDLL(_fullname)

# struct srm {
#   unsigned int width;
#   unsigned int height;
#   unsigned int size;
#   unsigned int channels;
#   uint8_t *in;
#   uint8_t *out;
#   unsigned int *sizes;
#   double logdelta;
#   unsigned int smallregion;
#   double g;
#   double Q;
#   struct unionfind *uf;
#   unsigned int borders;
#   struct pair *pairs;
#   unsigned int n_pairs;
#   struct pair *ordered_pairs;
#   unsigned int widthStep_in;
#   unsigned int widthStep_out;
# };

# this should be abstract
class _srm(C.Structure):
    _fields_ = [
        ("width",    C.c_uint),
        ("height",   C.c_uint),
        ("size",     C.c_uint),
        ("channels", C.c_uint),
        # more even more useless stuff
        ]

# struct srm* srm_new(
#                     double Q,
#                     unsigned int width,
#                     unsigned int height,
#                     unsigned int channels,
#                     unsigned int borders
#                    )
_lib.srm_new.restype  = C.POINTER(_srm)
_lib.srm_new.argtypes = [
    C.c_double,
    C.c_uint,
    C.c_uint,
    C.c_uint,
    C.c_uint,
    ]

# void srm_run(
#              struct srm *srm,
#              unsigned int widthStep_in,
#              uint8_t *in,
#              unsigned int widthStep_out,
#              uint8_t *out
#             )
_lib.srm_run.restype  = None
_lib.srm_run.argtypes = [
    C.POINTER(_srm),
    C.c_uint,
    npC.ndpointer(ndim=3, dtype=numpy.uint8),
    C.c_uint,
    npC.ndpointer(ndim=3, dtype=numpy.uint8),
    ]

# unsigned int srm_regions_count(
#                                struct srm *srm
#                               );
_lib.srm_regions.restype  = C.c_uint
_lib.srm_regions.argtypes = [
    C.POINTER(_srm),
    ]

# unsigned int* srm_regions(
#                           struct srm *srm
#                          );
_lib.srm_regions.restype  = C.POINTER(C.c_uint)
# _lib.srm_regions.restype  = npC.ndpointer(ndim=1, dtype=numpy.uint32, shape=(1,))
_lib.srm_regions.argtypes = [
    C.POINTER(_srm),
    ]

# unsigned int* srm_regions_sizes(
#                                 struct srm *srm
#                                );
_lib.srm_regions_sizes.restype  = C.POINTER(C.c_uint)
# _lib.srm_regions_sizes.restype  = npC.ndpointer(ndim=1, dtype=numpy.uint32, shape=(1,))
_lib.srm_regions_sizes.argtypes = [
    C.POINTER(_srm),
    ]

# void srm_delete(
#                 struct srm *srm
#                )
_lib.srm_delete.restype  = None
_lib.srm_delete.argtypes = [
    C.POINTER(_srm),
    ]

class SRM:
    # the _lib object seems to be destroyed before the SRM object, so we
    # remember the delete fucntion
    _srm_delete = _lib.srm_delete
    
    def __init__(self, Q, shape):
        self._srm = _lib.srm_new(Q, shape[0], shape[1], shape[2], 0)
        self._n = shape[0] * shape[1]
    
    def run(self, im, out=None):
        if out is None:
            out = numpy.empty_like(im)
        _lib.srm_run(self._srm, im.strides[0], im, out.strides[0], out)
        return out
    
    def count(self):
        return _lib.srm_regions_count(self._srm)
    
    def regions(self):
        return npC.as_array(_lib.srm_regions(self._srm), shape=(self._n,))
    
    def sizes(self):
        return npC.as_array(_lib.srm_regions_sizes(self._srm), shape=(self._n,))
    
    def __del__(self):
        self._srm_delete(self._srm)

if __name__ == "__main__":
    import sys
    from scipy.misc import imread
    from scipy.weave import inline
    from matplotlib import pyplot
    import numpy
    
    im = imread(sys.argv[1])
    n = im.shape[0] * im.shape[1]
    segmented = numpy.empty(im.shape, dtype=im.dtype)
    
    srm = SRM(512.0, im.shape)
    srm.run(im, segmented)
    
    count = srm.count()
    print count
    regions = srm.regions()
    regions_sizes = srm.sizes()
    
    # regions_id = numpy.empty(n, dtype=numpy.uint32)
    # sizes = numpy.zeros((count,),      dtype=numpy.uint32)
    # mu    = numpy.zeros((count, 5),    dtype=numpy.double)
    # Sigma = numpy.zeros((count, 5, 5), dtype=numpy.double)
    
    # with open("stats.cpp") as f:
    #     stats_code = f.read()
    # inline(stats_code,
    #        ['regions', 'regions_sizes', 'count', 'n', 'im', 
    #         'regions_id', 'sizes', 'mu', 'Sigma'],
    #        )
    
    # for i in xrange(count):
    #     print sizes[i]
    #     print mu[i]
    #     print Sigma[i]
    
    pyplot.imshow(segmented)
    pyplot.show()

