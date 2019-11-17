import os, sys
import json
from sklearn.metrics.cluster import normalized_mutual_info_score
from collections import OrderedDict
import copy

pred_dir = "/Users/daniel/Desktop/FSU/advanced_db/mstream/results/tweets"  # sys.argv[1]
labels_file = "/Users/daniel/Desktop/FSU/advanced_db/mstream/data/Tweets"  # sys.argv[2]

id2label = OrderedDict()
with open(labels_file) as input:
    for line in input:
        obj = json.loads(line)
        id2label[obj['Id']] = int(obj['clusterNo'])

id2pred = copy.deepcopy(id2label)
pred_files = os.listdir(pred_dir)
for f in pred_files:
    with open(pred_dir + "/" + f, "r") as input:
        for line in input:
            idx, cluster_pred = line.split('\t')
            id2pred[idx] = int(cluster_pred)


labels = []
preds = []

for k, v in id2label.items():
    labels.append(v)
for k, v in id2pred.items():
    preds.append(v)

nmi = normalized_mutual_info_score(labels, preds)

print("NMI is: ", nmi)

