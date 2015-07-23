PDFVIEWER=evince
OUTPUT_FILE=MPSS2015

show: build
	$(PDFVIEWER) $(OUTPUT_FILE).pdf &

plot:
	cd plots && build

convert:
	cd plots && make convert

build:
	@pdflatex -interaction=nonstopmode -halt-on-error -output-directory .output -jobname=$(OUTPUT_FILE) paper.tex 1>&2 > .output/error
	@mv .output/$(OUTPUT_FILE).pdf .

bibtex:
	@cd .output && bibtex paper && cd ..

clean:
	rm .output/*.aux .output/*.log
