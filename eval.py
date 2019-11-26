import os, sys
import json
from sklearn.metrics.cluster import normalized_mutual_info_score
from collections import OrderedDict
import copy
import matplotlib.pyplot as plt
import numpy as np

def get_labels(_labels_file):

    id2label = OrderedDict()
    with open(_labels_file) as input_f:
        for line in input_f:
            obj = json.loads(line)
            id2label[obj['Id']] = int(obj['clusterNo'])
    return id2label


def get_pred(pred_dir, _id2label):
    id2pred = copy.deepcopy(_id2label)
    pred_files = os.listdir(pred_dir)
    for f in pred_files:
        if f[0] == '.' or f == "summary.txt":
            continue
        with open(pred_dir + "/" + f, "r") as input_f:
            for line in input_f:
                idx, cluster_pred = line.split('\t')
                id2pred[idx] = int(cluster_pred)
    return id2pred


def calculate_nmi(_id2label, id2pred):
    labels = []
    preds = []
    for k, v in _id2label.items():
        labels.append(v)
    for k, v in id2pred.items():
        preds.append(v)
    nmi = normalized_mutual_info_score(labels, preds)
    print("NMI is: ", nmi)
    return nmi


def eval_iterations(_results_path, _id2label):
    outter_dir = _results_path + "/iterations"
    d_list = os.listdir(outter_dir)
    d_list = sorted(d_list, key=lambda x: int(x.split('_')[-1]))
    nmi_list = []
    num_clusters = []
    for i in d_list:
        inner_dir = outter_dir + "/" + i
        id2pred = get_pred(inner_dir, _id2label)
        nmi = calculate_nmi(_id2label, id2pred)
        nmi_list.append(nmi)
        with open(inner_dir+"/summary.txt", 'r') as f:
            for l in f:
                tokens = l.split()
                if tokens[0] == "ClusterCount:":
                    num_clusters.append(int(tokens[1]))
    return nmi_list, num_clusters

if __name__ == "__main__":
    dataset_name = "Tweets"
    results_path = "results/tweets"  # sys.argv[1]
    labels_file = "data/Tweets"  # sys.argv[2]
    id2label = get_labels(labels_file)
    nmi_list, num_clusters = eval_iterations(results_path, id2label)
    print(*nmi_list)
    print()
    print("max nmi: ", max(nmi_list), nmi_list.index(max(nmi_list)))
    print()
    print(*num_clusters)
    data = {'y_values': np.array(nmi_list),
            'x_axis_line': np.array(range(1,1+len(nmi_list)))
            }
    print(len(nmi_list))
    plt.title(dataset_name)
    plt.plot('x_axis_line', 'y_values', 'bo-', data=data)
    plt.axis([0, 11, 0, 1])
    plt.grid(True)
    plt.xlabel('Iterations')
    plt.ylabel('NMI')
    plt.show()
    data = {'y_values': np.array(num_clusters),
            'x_axis_line': np.array(range(1,1+len(num_clusters)))
            }
    plt.title(dataset_name)
    plt.plot('x_axis_line', 'y_values', 'bo-', data=data)
    plt.xticks(np.arange(0, 11, 2.0))
    plt.yticks(np.arange(0,500, 50))
    #plt.axis([0, 11, 0, 500])
    plt.grid(True)
    plt.xlabel('Iterations')
    plt.ylabel('Number of Clusters')
    plt.show()

