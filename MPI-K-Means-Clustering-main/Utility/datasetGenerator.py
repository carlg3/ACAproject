# it creates a MxN matrix of random values -- carlg
import numpy as np

def write_matrix(M, N):
    fpath = f".\\src\\dataset_{M}x{N}.txt"

    matrix = np.random.randint(0, 10, (M, N))
    np.savetxt(fpath, matrix, fmt = "%d", delimiter = ",")

write_matrix(100, 10)