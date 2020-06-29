#!/user/bin/env python

import sys
import nibabel as nib
import numpy as np

from os import path as op

in_file=sys.argv[1]
out_folder=sys.argv[2]
it=int(sys.argv[3])

for i in range(it):
    if i == 0:
        out_f = op.join(out_folder, '{0}-inc-{1}'.format(i, op.basename(in_file)))
    else:
        out_f = op.join(out_folder, '{0}-{1}'.format(i, '-'.join(op.basename(in_file).split('-')[1:])))
    #print("iteration", i, "saving to", out_f)

    im = nib.load(in_file)
    inc = np.asanyarray(im.get_fdata()) + 1
    inc_im = nib.Nifti1Image(inc, im.affine, header=im.header)

    nib.save(inc_im, out_f)
    in_file = out_f

