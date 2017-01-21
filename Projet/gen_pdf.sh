
for f in `ls FICHIERS_GRAPHVIZ`; do
  dot -Tpdf FICHIERS_GRAPHVIZ/$f -o FICHIERS_PLOT_GRAPHVIZ/$f.pdf
done