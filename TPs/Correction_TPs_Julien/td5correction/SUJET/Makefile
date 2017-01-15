TARGET=td5

all: $(TARGET) $(TARGET)_correction

$(TARGET):
	pdflatex --jobname=$(TARGET) $(TARGET) 

$(TARGET)_correction:
	pdflatex --jobname=$(TARGET)_correction "\def\tpcorrection{YES} \include{$(TARGET)}"

clean_tmp:
	rm -f *.aux *.log *.out

clean:
	rm -f *.pdf *.aux *.log *.out
