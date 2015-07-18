set terminal png
set output "graphs.png"
set datafile separator "\t"

set boxwidth 0.5
set style fill solid
set key top center
set key at 8.0,1.1

set style line 1 lc rgb "red"
set style line 2 lc rgb "blue"
set style line 3 lc rgb "green"
set style line 4 lc rgb "yellow"
set style line 5 lc rgb "purple"

set style line 1 lt 1 lc rgb '#0072bd' # blue
set style line 2 lt 1 lc rgb '#d95319' # orange
set style line 3 lt 1 lc rgb '#edb120' # yellow
set style line 4 lt 1 lc rgb '#7e2f8e' # purple
set style line 5 lt 1 lc rgb '#77ac30' # green


set yrange [0:1.1]
set xtics ( \
    "Accuracy" 0.75, \
    "Precision ¬SC" 4.75, \
    "Precision SC" 8.75, \
    "Recall ¬SC" 12.75, \
    "Recall SC" 16.75)
#set ytics (0.0, 0.2, 0.4, 0.6, 0.8, 1.0)


plot 'plot.csv' every 4::0 using 1:2 with boxes ls 1 title "Majority Voting", \
     'plot.csv' every 4::1 using 1:2 with boxes ls 2 title "First", \
     'plot.csv' every 4::2 using 1:2 with boxes ls 3 title "Last-Seq", \
     'plot.csv' every 4::3 using 1:2 with boxes ls 4 title "Last-Frame"



