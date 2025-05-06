### K-means algorithm

The k-means clustering is an algorithm with the aim to group similar data points to clusters via vector quantization.
It uses an interative refinement technique carried out in 2 main steps:

#### 1. Assignemt step:

Assign each data point to the cluster with the nearest mean (the least squared Euclidean distance).

#### 2. Update step:

Recalculate means (centroids) for data points assigned to each cluster.

The steps get repeated until the cluster don't change anymore (no data point gets reassigned to a different cluster).
