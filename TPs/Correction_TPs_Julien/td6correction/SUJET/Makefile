TARGET=td6

all: $(TARGET) $(TARGET)_correction

$(TARGET):
	pdflatex --jobname=$(TARGET) $(TARGET) 

$(TARGET)_correction:
	pdflatex --jobname=$(TARGET)_correction "\def\tpcorrection{YES} \include{$(TARGET)}"

clean:
	rm -f *.pdf *.aux *.log *.out
