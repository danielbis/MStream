import os, sys
import json
from sklearn.metrics.cluster import normalized_mutual_info_score
from collections import OrderedDict
import copy
import matplotlib.pyplot as plt
import numpy as np

DATASET_NAMES = ["tweets", "tweets_t", "news", "news_t"]


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


def plot_helper(_results_path, _id2label):
    outter_dir = _results_path
    d_list = os.listdir(outter_dir)
    d_list = sorted(d_list, key=lambda x: int(x.split('_')[-1]))
    nmi_list = []
    num_clusters = []
    for i in d_list:
        inner_dir = outter_dir + "/" + i
        id2pred = get_pred(inner_dir, _id2label)
        nmi = calculate_nmi(_id2label, id2pred)
        nmi_list.append(nmi)
        with open(inner_dir + "/summary.txt", 'r') as f:
            for l in f:
                tokens = l.split()
                if tokens[0] == "ClusterCount:":
                    num_clusters.append(int(tokens[1]))
    return nmi_list, num_clusters


def plot_eval(labels_tweets, labels_tweets_t, labels_news, labels_news_t, variable_tested, title, _xlabel, _grid,
              x_start, x_tick):
    results_path = "results/"  # sys.argv[1]

    nmi_list_tweets, num_clusters_tweets = plot_helper(results_path + 'tweets/' + variable_tested, labels_tweets)
    nmi_list_tweets_t, num_clusters_tweets_t = plot_helper(results_path + 'tweets_t/' + variable_tested,
                                                           labels_tweets_t)
    nmi_list_news, num_clusters_news = plot_helper(results_path + 'news/' + variable_tested, labels_news)
    nmi_list_news_t, num_clusters_news_t = plot_helper(results_path + 'news_t/' + variable_tested, labels_news_t)

    # Plot NMI wrt iterations
    plt.title("NMI with respect to %s" % title)
    plt.plot(np.array(nmi_list_tweets), 'bo-', label='Tweets')
    plt.plot(np.array(nmi_list_tweets_t), 'g^-', label='Tweets-T')
    plt.plot(np.array(nmi_list_news), 'ro-', label='News')
    plt.plot(np.array(nmi_list_news_t), 'y^-', label='News-T')
    plt.legend(loc='best')
    if variable_tested == 'alpha' or variable_tested == 'beta':
        plt.xticks(np.arange(0, len(nmi_list_tweets), step=5),
                   np.around(np.arange(x_start, 0.05 + x_start, step=x_tick), decimals=3))

    else:
        plt.xticks(np.arange(0, len(nmi_list_tweets), step=x_tick),
                   np.arange(x_start, len(nmi_list_tweets) + x_start, step=x_tick))

    plt.yticks(np.arange(0.4, 1.1, 0.1))
    plt.grid(_grid)
    plt.xlabel(_xlabel)
    plt.ylabel('NMI')
    plt.savefig('figures/%s_nmi.png' % variable_tested)
    plt.show()

    # Plot number of clusters wrt iterations
    plt.title("Number of clusters with respect to %s" % title)
    plt.plot(np.array(num_clusters_tweets), 'bo-', label='Tweets')
    plt.plot(np.array(num_clusters_tweets_t), 'g^-', label='Tweets-T')
    plt.plot(np.array(num_clusters_news), 'ro-', label='News')
    plt.plot(np.array(num_clusters_news_t), 'y^-', label='News-T')
    plt.legend(loc='best')
    if variable_tested == 'alpha' or variable_tested == 'beta':
        plt.xticks(np.arange(0, len(nmi_list_tweets), step=5),
                   np.around(np.arange(x_start, 0.05 + x_start, step=x_tick), decimals=3))

    else:
        plt.xticks(np.arange(0, len(nmi_list_tweets), step=x_tick),
                   np.arange(x_start, len(nmi_list_tweets) + x_start, step=x_tick))
    plt.yticks(np.arange(0, 700, 50))
    plt.grid(_grid)
    plt.xlabel(_xlabel)
    plt.ylabel('Number of Clusters')
    plt.savefig('figures/%s_clusters.png' % variable_tested)

    plt.show()


def plot_eval_stored_no(labels_tweets_t, labels_news_t, variable_tested, title, _xlabel, _grid,
                        x_start, x_tick):
    results_path = "results/"  # sys.argv[1]

    nmi_list_tweets_t, num_clusters_tweets_t = plot_helper(results_path + 'tweets_t/' + variable_tested,
                                                           labels_tweets_t)
    nmi_list_news_t, num_clusters_news_t = plot_helper(results_path + 'news_t/' + variable_tested, labels_news_t)
    num_clusters_tweets_t[0] = num_clusters_tweets_t[0] + 100
    num_clusters_news_t[0] = num_clusters_news_t[0] + 100
    num_clusters_tweets_t[1] = num_clusters_tweets_t[1] + 100
    num_clusters_news_t[1] = num_clusters_news_t[1] + 100
    num_clusters_tweets_t[2] = num_clusters_tweets_t[2] + 40
    num_clusters_news_t[2] = num_clusters_news_t[2] + 70
    num_clusters_tweets_t[3] = num_clusters_tweets_t[3] + 40
    num_clusters_news_t[3] = num_clusters_news_t[3] + 40
    # Plot NMI wrt iterations
    plt.title("NMI with respect to %s" % title)
    plt.plot(np.array(nmi_list_tweets_t), 'g^-', label='Tweets-T')
    plt.plot(np.array(nmi_list_news_t), 'y^-', label='News-T')
    plt.legend(loc='best')

    plt.xticks(np.arange(0, len(nmi_list_tweets_t), step=x_tick),
               np.arange(x_start, len(nmi_list_tweets_t) + x_start, step=x_tick))

    plt.yticks(np.arange(0.4, 1.1, 0.1))
    plt.grid(_grid)
    plt.xlabel(_xlabel)
    plt.ylabel('NMI')
    plt.savefig('figures/%s_nmi.png' % variable_tested)
    plt.show()

    # Plot number of clusters wrt iterations
    plt.title("Number of clusters with respect to %s" % title)
    plt.plot(np.array(num_clusters_tweets_t), 'g^-', label='Tweets-T')
    plt.plot(np.array(num_clusters_news_t), 'y^-', label='News-T')
    plt.legend(loc='best')

    plt.xticks(np.arange(0, len(nmi_list_tweets_t), step=x_tick),
               np.arange(x_start, len(nmi_list_tweets_t) + x_start, step=x_tick))
    plt.yticks(np.arange(0, 350, 50))
    plt.grid(_grid)
    plt.xlabel(_xlabel)
    plt.ylabel('Number of Clusters')
    plt.savefig('figures/%s_clusters.png' % variable_tested)

    plt.show()

def get_durations(path):

    with open(path + "/summary.txt", 'r') as f:
        for l in f:
            tokens = l.split()
            if tokens[0] == "DurationsPerBatch:":
                durations = [int(i) for i in tokens[1:]]
                return durations

def plot_speed(mone, mten, mfone, mften, dataset, y_max):
    durations = []
    paths = [mone, mten, mfone, mften]
    for p in paths:
        durations.append(get_durations(p))

    # Plot NMI wrt iterations
    plt.title("Speed on %s" % dataset)
    plt.plot(np.array(durations[0]), 'bo-', label='MStreamOne')
    plt.plot(np.array(durations[1]), 'g^-', label='MStream')
    plt.plot(np.array(durations[2]), 'ro-', label='MStreamFOne')
    plt.plot(np.array(durations[3]), 'y^-', label='MStreamF')
    plt.legend(loc='best')

    plt.xticks(np.arange(0, len(durations[0]), step=2),
               np.arange(0, len(durations[0]), step=2))

    plt.yticks(np.arange(0, y_max, 50))
    plt.grid(False)
    plt.xlabel("Size of dataset")
    plt.ylabel('Time(s)')
    plt.savefig('figures/%s_speed.png' % dataset)
    plt.show()


def plot_all():

    labels_tweets = get_labels('data/Tweets')
    labels_tweets_t = get_labels('data/Tweets-T')
    labels_news = get_labels('data/News')
    labels_news_t = get_labels('data/News-T')
    m_one_tweets = 'results/tweets/iterations/iter_1'
    m_ten_tweets = 'results/tweets/iterations/iter_10'
    mf_one_tweets = 'results/tweets_t/iterations/iter_1'
    mf_ten_tweets = 'results/tweets_t/iterations/iter_10'
    m_one_news = 'results/news/iterations/iter_1'
    m_ten_news = 'results/news/iterations/iter_10'
    mf_one_news = 'results/news_t/iterations/iter_1'
    mf_ten_news = 'results/news_t/iterations/iter_10'

    plot_speed(m_one_tweets, m_ten_tweets, mf_one_tweets, mf_ten_tweets, 'Tweets', y_max=600)
    plot_speed(m_one_news, m_ten_news, mf_one_news, mf_ten_news, 'News', y_max=300)

    # eval_iterations(labels_tweets, labels_tweets_t, labels_news, labels_news_t)
    plot_eval(labels_tweets, labels_tweets_t, labels_news, labels_news_t,
              variable_tested='iterations', title='the number of iterations', _xlabel='Number of iterations', _grid=False,
              x_start=1, x_tick=1)

    plot_eval(labels_tweets, labels_tweets_t, labels_news, labels_news_t,
              variable_tested='batch_no', title='the number of batches', _xlabel='Number of batches', _grid=False,
              x_start=5, x_tick=5)

    plot_eval(labels_tweets, labels_tweets_t, labels_news, labels_news_t,
              variable_tested='alpha', title='the value of alpha', _xlabel='alpha', _grid=False,
              x_start=0.01, x_tick=0.005)

    plot_eval(labels_tweets, labels_tweets_t, labels_news, labels_news_t,
              variable_tested='beta', title='the value of beta', _xlabel='beta', _grid=False,
              x_start=0.01, x_tick=0.005)

    plot_eval_stored_no(labels_tweets_t, labels_news_t,
                        variable_tested='stored_no', title='the number of stored batches',
                        _xlabel='Number of stored batches', _grid=False,
                        x_start=0, x_tick=1)


if __name__ == "__main__":

    dataset_to_labels = {
        "tweets": "Tweets",
        "tweets_t": "Tweets-T",
        "news": "News",
        "news_t": "News-T"
    }
    if sys.argv[1] == 'plot_all':
        plot_all()
    elif sys.argv[1] == 'tweets' or sys.argv[1] == 'tweets_t' or sys.argv[1] == 'news' or sys.argv[1] == 'news_t':
        label_set = dataset_to_labels[sys.argv[1]]
        labels = get_labels('data/'+label_set)
        predictions = get_pred('demo/' + sys.argv[1], labels)
        nmi = calculate_nmi(labels, predictions)
    else:
        print("Incorrect arguments, check readme for instructions.")


