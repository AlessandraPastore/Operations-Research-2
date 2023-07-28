@echo off

REM Run xelatex
xelatex tesi.tex

REM Run bibtex
bibtex tesi

REM Run xelatex again (to incorporate bibliography)
xelatex tesi.tex

REM Run xelatex one more time (to update references)
xelatex tesi.tex