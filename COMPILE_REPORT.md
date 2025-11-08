# How to Compile the LaTeX Report

## Quick Start

To compile the LaTeX report to PDF:

```bash
cd "/home/depk/Academics/AM5801 - Computational Lab/Final Exam"
pdflatex FEM_End_Sem_Report.tex
pdflatex FEM_End_Sem_Report.tex
```

Run `pdflatex` twice to ensure all references and citations are properly resolved.

## Alternative: Using latexmk

If you have `latexmk` installed:

```bash
latexmk -pdf FEM_End_Sem_Report.tex
```

This automatically runs pdflatex the correct number of times.

## What's Included in the Report

The LaTeX document (`FEM_End_Sem_Report.tex`) contains:

1. **Title Page** with authors (Deepak S and Muskan) and course info
2. **Abstract** summarizing the entire study
3. **Introduction** explaining why this problem is interesting
4. **Problem Definition** with all five load cases
5. **Solution Strategy** describing LU decomposition for multiple loads
6. **Numerical Methods** - detailed descriptions of all six solvers
7. **Implementation Details** - system configuration, software architecture, sparsity analysis
8. **Results and Analysis** - performance tables, complexity analysis, displacement/stress results
9. **Discussion** - why sparse methods win, when to use each method
10. **Literature Review** - validation against published papers
11. **Conclusions** - key findings and take-home messages
12. **References** - complete bibliography
13. **GitHub Repository Link** throughout the document

## Files Created

- `FEM_End_Sem_Report.tex` - Main LaTeX source (comprehensive end semester report)
- `fem_solver_analysis.ipynb` - Updated notebook with GitHub link and author names
- `REFERENCES_NOTEBOOK.md` - Bibliography for notebook

## Output

After compilation, you'll get:
- `FEM_End_Sem_Report.pdf` - Your final report (ready to submit!)
- `FEM_End_Sem_Report.aux`, `.log`, `.out`, `.toc` - LaTeX auxiliary files

## Troubleshooting

If compilation fails:
1. Make sure you have a LaTeX distribution installed (TeX Live, MiKTeX, etc.)
2. Check that all required packages are installed
3. Look at the `.log` file for specific error messages

## Quick Commands

```bash
# Compile the report
pdflatex FEM_End_Sem_Report.tex

# View the PDF (Linux)
xdg-open FEM_End_Sem_Report.pdf

# Clean auxiliary files
rm *.aux *.log *.out *.toc
```

## Authors

- Deepak S (MM22B011)
- Muskan (BE22B031)

Course: AM5801 - Computational Lab  
Date: November 8, 2025

GitHub: https://github.com/mm22b011-deepaks/AM5801-FInal-End-Sem-Submission
