$(function() {
  var margin = {top: 40, right: 20, bottom: 30, left: 40},
      width = 10000 - margin.left - margin.right,
      height = 500 - margin.top - margin.bottom,
      barWidth = 1;

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
      return d;
    })

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
        .attr("class", function(d) { return d.gold ? "bar-cut" : "bar" })
        .attr("x", function(d) { return x(d.idx) * barWidth; })
        .attr("width", barWidth)
        .attr("y", function(d) { return y(d.absDiff); })
        .attr("height", function(d) { return height - y(d.absDiff); })
        .on("mouseover", function(d) {
          var path1 = '../frames/' + d.frame1;
          var path2 = '../frames/' + d.frame2;
          d3.select("#frame1")
            .attr("src", path1);
          d3.select("#frame2")
            .attr("src", path2);
          tip.show(d);
        })
        .on("mouseout", tip.hide);

  });

  function type(d) {
    d.absDiff = +d.absDiff;
    return d;
  }
});