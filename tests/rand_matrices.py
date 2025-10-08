import numpy as np

a = np.random.rand(3, 4, 5)
b = np.random.rand(4, 3, 2)

np.save("a.npy", a)
np.save("b.npy", b)
