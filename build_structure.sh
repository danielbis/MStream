#!/bin/bash
mkdir results
mkdir results/tweets
mkdir results/tweets_t
mkdir results/news
mkdir results/news_t

mkdir results/tweets/iterations
mkdir results/tweets_t/iterations
mkdir results/news/iterations
mkdir results/news_t/iterations

mkdir results/tweets/batch_no
mkdir results/tweets_t/batch_no
mkdir results/news/batch_no
mkdir results/news_t/batch_no

mkdir results/tweets/alpha
mkdir results/tweets_t/alpha
mkdir results/news/alpha
mkdir results/news_t/alpha

mkdir results/tweets/beta
mkdir results/tweets_t/beta
mkdir results/news/beta
mkdir results/news_t/beta

mkdir results/tweets_t/stored_no
mkdir results/news_t/stored_no

for i in {1..10}
do
    mkdir results/tweets/iterations/iter_${i}
    mkdir results/tweets_t/iterations/iter_${i}
    mkdir results/news/iterations/iter_${i}
    mkdir results/news_t/iterations/iter_${i}
done

for i in {5..30}
do
    mkdir results/tweets/batch_no/batches_${i}
    mkdir results/tweets_t/batch_no/batches_${i}
    mkdir results/news/batch_no/batches_${i}
    mkdir results/news_t/batch_no/batches_${i}
done

for i in {10..50}
do
    mkdir results/tweets/alpha/a_${i}
    mkdir results/tweets_t/alpha/a_${i}
    mkdir results/news/alpha/a_${i}
    mkdir results/news_t/alpha/a_${i}
done

for i in {10..50}
do
    mkdir results/tweets/beta/b_${i}
    mkdir results/tweets_t/beta/b_${i}
    mkdir results/news/beta/b_${i}
    mkdir results/news_t/beta/b_${i}
done

mkdir results/tweets_t/stored_no/s_0
mkdir results/news_t/stored_no/s_0
for i in {1..16}
do
    mkdir results/tweets_t/stored_no/s_${i}
    mkdir results/news_t/stored_no/s_${i}
done