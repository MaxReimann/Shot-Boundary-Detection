$(function() {
  var margin = {top: 40, right: 20, bottom: 30, left: 40},
      width = 10000 - margin.left - margin.right,
      height = 500 - margin.top - margin.bottom,
      barWidth = 1;

  var loaded1, loaded2;

  var autoDiff = document.getElementById("autoDiff");
  autoDiff.addEventListener("click", function() {
    if (autoDiff.checked) {
      drawDiff();
    }
  });

  var x = d3.scale.linear();

  var y = d3.scale.linear()
      .range([height, 0]);

  var xAxis = d3.svg.axis()
      .scale(x)
      .orient("bottom");

  var yAxis = d3.svg.axis()
      .scale(y)
      .orient("left")

  var tip = d3.tip()
    .attr('class', 'd3-tip')
    .offset([-10, 0])
    .html(function(d) {
      return "<strong>Transition:</strong> <span style='color:red'>" + d.frame1 + " - " + d.frame2 + "</span>";
    })  

  d3.tsv("./data/visData.tsv", type, function(error, data) {
    var svg = d3.select("#svg-container").append("svg")
      .attr("width", data.length * barWidth + margin.left + margin.right)
      .attr("height", height + margin.top + margin.bottom)
    .append("g")
      .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

    svg.call(tip);
    data = data.map(function(d) { 
      d.idx = parseInt(d.idx);
      d.gold = d.gold == "1" ? true : false;
      d.prediction = d.prediction == "1" ? true : false;
      return d;
    });

    x.domain(data.map(function(d) { return d.idx; }));
    y.domain([0, d3.max(data, function(d) { return d.absDiff; })]);

    svg.append("g")
        .attr("class", "x axis")
        .attr("transform", "translate(0," + height + ")")
        .call(xAxis);

    svg.append("g")
        .attr("class", "y axis")
        .call(yAxis)
      .append("text")
        .attr("transform", "rotate(-90)")
        .attr("y", 6)
        .attr("dy", ".71em")
        .style("text-anchor", "end")
        .text("Difference");

    svg.selectAll(".bar")
        .data(data)
      .enter().append("rect")
        .attr("class", function(d) { 
          if (d.gold && d.prediction) return "bar-tp";
          if (!d.gold && !d.prediction) return "bar-tn";
          if (d.gold && !d.prediction) return "bar-fn";
          if (!d.gold && d.prediction) return "bar-fp";
        })
        .attr("x", function(d) { return x(d.idx) * barWidth; })
        .attr("width", barWidth)
        .attr("y", function(d) { return y(d.absDiff); })
        .attr("height", function(d) { return height - y(d.absDiff); })
        .on("mouseover", function(d) {
          var path1 = '../test2/frames/' + d.frame1;
          var path2 = '../test2/frames/' + d.frame2;
          d3.select("#frame1")
            .on("load", function() {
              loaded1 = true;
              if (loaded1 && loaded2 && autoDiff.checked) {
                drawDiff();
              }
            })
            .attr("src", path1);
          d3.select("#frame2")
            .on("load", function() {
              loaded2 = true;
              if (loaded1 && loaded2 && autoDiff.checked) {
                drawDiff();
              }
            })
            .attr("src", path2);
          tip.show(d);
        })
        .on("mouseout", tip.hide);

  });

  function type(d) {
    d.absDiff = +d.absDiff;
    return d;
  }

  function drawDiff() {
    loaded1 = false;
    loaded2 = false;

    var frame1 = document.getElementById("frame1");
    var frame2 = document.getElementById("frame2");

    var cvs1 = document.createElement("Canvas");
    cvs1.width = frame1.width;
    cvs1.height = frame1.height;
    var ctx1 = cvs1.getContext("2d");

    var cvs2 = document.createElement("Canvas");
    cvs2.width = frame2.width;
    cvs2.height = frame2.height;
    var ctx2 = cvs2.getContext("2d");

    ctx1.drawImage(frame1, 0, 0);
    ctx2.drawImage(frame2, 0, 0);

    var data1 = ctx1.getImageData(0, 0, cvs1.width, cvs1.height).data;
    var data2 = ctx2.getImageData(0, 0, cvs2.width, cvs2.height).data;

    var diff = new Uint8Array(data1.length);
    for (var i = 0; i < data1.length; i++) {
        if (i % 4 == 3) {
            // alpha channel
            diff[i] = 255;
        } else {
            diff[i] = Math.abs(data1[i] - data2[i]);
        }
    }

    var diffCvs = document.getElementById("diff");
    diffCvs.width = frame1.width;
    diffCvs.height = frame1.height;
    var diffCtx = diffCvs.getContext("2d");
    var imgData = diffCtx.createImageData(frame1.width, frame1.height);
    imgData.data.set(diff);
    diffCtx.putImageData(imgData, 0, 0);
  }

});