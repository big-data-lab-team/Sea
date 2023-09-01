#!/usr/bin/env python
import os.path as op
import nibabel as nib
import matplotlib.pyplot as plt

from glob import glob
from numpy import histogram, asarray, int16, save, load
from pydra import mark, Workflow, Submitter

import click
from typing import Tuple


def merge_hist(filenames: list) -> None:
    hist_dict = {}

    for res in filenames:
        bins = load(res.output.result[1])
        for i, v in enumerate(load(res.output.result[0])):
            if bins[i] not in hist_dict:
                hist_dict[bins[i]] = v
            else:
                hist_dict[bins[i]] += v

    plt.hist(list(hist_dict.values()), bins=list(hist_dict.keys()))
    plt.savefig("hist.pdf")


@mark.task
@mark.annotate({"return": {"outfiles": Tuple[str, str]}})
def histo(filename: str, outdir: str) -> Tuple[str, str]:
    sigma = 5

    img = nib.load(filename)
    data = asarray(img.get_fdata(), dtype=int16)
    hist, bins = histogram(data, bins=3)

    histname = op.join(outdir, f"hist_{op.basename(filename)}.npy")
    binname = op.join(outdir, f"bin_{op.basename(filename)}.npy")

    save(histname, hist)
    save(binname, bins)

    return (histname, binname)


@click.command()
@click.argument("dir", type=click.Path(exists=True, resolve_path=True))
@click.argument("output_dir", type=click.Path(resolve_path=True))
@click.option("--range", nargs=2, type=int, default=None)
def main(dir, output_dir, range):

    paths = glob(op.join(dir, "*"))

    if range is not None:
        paths = paths[range[0] : range[1]]

    wf = Workflow(name="histobb", input_spec=["fn", "outdir"], outdir=output_dir)
    wf.split(["fn"], fn=paths)
    wf.add(histo(name="histogram", filename=wf.lzin.fn, outdir=wf.lzin.outdir))
    wf.set_output([("result", wf.histogram.lzout.outfiles)])

    with Submitter(
        plugin="cf",
    ) as sub:
        sub(wf)

    merge_hist(wf.result())


if __name__ == "__main__":
    main()
