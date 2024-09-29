
dataset=t2i-10M
prefix=/root/datasets/${dataset}
save_prefix=/root/indices/${dataset}

cd build

./tests/test_taumng_index ${prefix}/base.10M.fbin ${save_prefix}/${dataset}.knn 500 64 500 0.01 60 ${save_prefix}/${dataset}.mng