
./Word2Vec skipgram -input /data/mszhang/ACL2017-Word2Vec/data/enwiki-20150112_text.txt -output context/enwiki.emb -lr 0.025 -dim 100  -ws 5 -epoch 5 -minCount 100 -neg 5 -loss ns  -minn 3 -maxn 6 -thread 12 -t 1e-4 -lrUpdateRate 100 > context.log 2>&1


