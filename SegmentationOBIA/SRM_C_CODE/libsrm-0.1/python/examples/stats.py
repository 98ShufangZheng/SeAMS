import sys
from scipy.misc import imread
from scipy.weave import inline
from matplotlib import pyplot
import numpy

from srm import SRM

im = imread(sys.argv[1])
n = im.shape[0] * im.shape[1]
segmented = numpy.empty(im.shape, dtype=im.dtype)
    
srm = SRM(512.0, im.shape)
srm.run(im, segmented)
    
count = srm.count()
print count
regions = srm.regions()
regions_sizes = srm.sizes()
    
regions_id = numpy.empty(n, dtype=numpy.uint32)
sizes = numpy.zeros((count,),      dtype=numpy.uint32)
mu    = numpy.zeros((count, 5),    dtype=numpy.double)
Sigma = numpy.zeros((count, 5, 5), dtype=numpy.double)

with open("pystats.cpp") as f:
    stats_code = f.read()
    inline(stats_code,
           ['regions', 'regions_sizes', 'count', 'n', 'im', 
            'regions_id', 'sizes', 'mu', 'Sigma'],
           )
    
    for i in xrange(count):
        print sizes[i]
        print mu[i]
        print Sigma[i]
    
    pyplot.imshow(segmented)
    pyplot.show()
