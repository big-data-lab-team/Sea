#!/usr/bin/env python
import os.path as op
import nibabel as nib
from glob import glob
from scipy.ndimage import gaussian_filter
from pydra import mark, Workflow, Submitter

import click
from numpy.typing import NDArray


@mark.task
@mark.annotate({"return": {"outfile": str}})
def filter(filename: str, outdir: str) -> str:
    sigma = 5

    img = nib.load(filename)
    data = img.get_fdata()
    smooth = gaussian_filter(data, sigma=sigma)
    smooth_img = nib.Nifti1Image(smooth, img.affine, img.header)

    outfile = op.join(outdir, f"smooth{sigma}_{op.basename(filename)}")

    nib.save(smooth_img, outfile)

    return outfile


@click.command()
@click.argument("dir", type=click.Path(exists=True, resolve_path=True))
@click.argument("output_dir", type=click.Path(resolve_path=True))
@click.option("--range", nargs=2, type=int, default=None)
def main(dir, output_dir, range):

    paths = glob(op.join(dir, "*"))

    if range is not None:
        paths = paths[range[0] : range[1]]

    wf = Workflow(name="filterbb", input_spec=["fn", "outdir"], outdir=output_dir)
    wf.split(["fn"], fn=paths)
    wf.add(filter(name="smooth", filename=wf.lzin.fn, outdir=wf.lzin.outdir))
    wf.set_output([("result", wf.smooth.lzout.outfile)])

    with Submitter(plugin="cf") as sub:
        sub(wf)

    for res in wf.result():
        print(res.output.result)


if __name__ == "__main__":
    main()
