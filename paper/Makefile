pdfviewer=evince

show: build
	$(pdfviewer) paper.pdf &

plot:
	cd plots && build

convert:
	cd plots && make convert

build:
	@pdflatex -interaction=nonstopmode -halt-on-error -output-directory .output paper.tex 1>&2 > .output/error
	@mv .output/paper.pdf .

bibtex:
	@cd .output && bibtex paper && cd ..

clean:
	rm .output/*.aux .output/*.log
