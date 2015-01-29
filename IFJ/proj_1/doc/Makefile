NAME=ifj_doc

$(NAME).pdf: $(NAME).dvi
	dvipdf -dAutoRotatePages=/None $(NAME).dvi

$(NAME).dvi: $(NAME).tex title.tex
	make -C img
	latex $(NAME).tex
	latex $(NAME).tex

clean:
	-rm $(NAME).dvi $(NAME).aux $(NAME).toc $(NAME).log $(NAME).out
