#!/usr/bin/env python
import os.path as op
from typing import Tuple
import nibabel as nib
import numpy as np
from glob import glob
from pydra import mark, Workflow, Submitter

import click
from numpy.typing import NDArray


@mark.task
@mark.annotate({"return": {"centroids": list}})
def update_centroids(assignments: list, centroids: list) -> list:
    new_centroids = []
    for i in range(len(centroids)):
        num_elements = 0
        cum_sum = 0

        for f in assignments:
            img = nib.load(f[0])
            data = np.asanyarray(img.get_fdata(), dtype=np.uint16)

            a = np.load(f[1])
            assigned_values = data[a == i]
            cum_sum += np.sum(assigned_values)
            num_elements += assigned_values.size

        print(i, cum_sum, num_elements)
        if num_elements == 0:
            cum_sum = centroids[i]
            num_elements = 1

        new_centroids.append(cum_sum / num_elements)

    return new_centroids


@mark.task
@mark.annotate({"return": {"outfiles": Tuple[str, str]}})
def assign_centroids(filename: str, centroids: list, outdir: str) -> Tuple[str, str]:
    img = nib.load(filename)
    data = np.asanyarray(img.get_fdata(), dtype=np.uint16)
    a = abs(data - centroids[0])
    assignments = np.zeros(shape=a.shape, dtype=np.uint16)

    for i, c in enumerate(centroids[1:]):
        b = abs(data - c)
        assignments = np.where(a < b, assignments, i + 1)
        a = np.minimum(a, b)

    assignment_fn = op.join(outdir, f"assignments_{op.basename(filename)}.npy")
    np.save(assignment_fn, assignments)
    return (filename, assignment_fn)


def execute_wf(paths: list, centroids: list, output_dir: str) -> list:
    wf = Workflow(
        name="kmeansbb",
        input_spec=["fn", "centroids", "outdir"],
        fn=paths,
        outdir=output_dir,
    )
    wf.add(
        assign_centroids(
            name="assign",
            filename=wf.lzin.fn,
            centroids=centroids,
            outdir=wf.lzin.outdir,
        )
        .split("assign.filename")
        .combine("assign.filename")
    )
    wf.add(
        update_centroids(
            name="update",
            assignments=wf.assign.lzout.outfiles,
            centroids=centroids,
        )
    )
    wf.set_output([("result", wf.update.lzout.centroids)])

    with Submitter(plugin="cf") as sub:
        sub(wf)

    new_centroids = wf.result().output.result
    return new_centroids


@click.command()
@click.argument("indir", type=click.Path(exists=True, resolve_path=True))
@click.argument("output_dir", type=click.Path(resolve_path=True))
@click.option("--range", nargs=2, type=int, default=None)
@click.option("--centroids", nargs=4, type=int, default=None)
def main(indir, output_dir, range, centroids):

    paths = glob(op.join(indir, "*"))

    if range is not None:
        paths = paths[range[0] : range[1]]

    while True:
        new_centroids = execute_wf(paths, centroids, output_dir)

        if centroids == new_centroids:
            break

        centroids = new_centroids

    print(f"Final centroids: {new_centroids}")


if __name__ == "__main__":
    main()
