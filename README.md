FCS - Finding Compact Set

Table of Contents
=================

- Installation
- Data Format
- 'fcs' Usage
- Additional Information

Installation
============================

On Unix systems,
>cd source <br>
>make

Run them without arguments to show the usages of them.

Data Format
============================

Input = distance matrix whose format the same with one for phylip:

4<br>
seq1 0 4.1 5.1 5.1<br>
seq2 4.1 0 7.1 3.1<br>
seq3 5.1 7.1 0 9.1<br>
seq4 5.1 3.1 9.1 0<br>

Output = compact set relation in newick format:

(seq3,(seq4,seq2),seq1);

'fcs' Usage
=================

Usage: fcs distance_matrix_file compact-set_output_file

For instance:
>fcs example/distance1.txt example/cs1.ph

running information: <br>
[SUMMARY]<br>
\# of veterx = 4<br>
\# of edges  = 7 6<br>
compact set = seq2,seq4,<br>

Additional Information
======================

Reference: 
Kim, SK. 1996. A note on finding compact sets in graphs represented by an adjacency list. Information Processing Letters 57:335-338.
T.H. Corman, C.E. eiserson and R.L. Rivest, 1994, Introduction to Algorithms,ch.22 24

If you find FCS helpful, please cite it as
Chang, JM, CY Tang, CL Chang. 2002. Compact Set Neighboring Relation and Its Application in the Evaluating the Evolution Tree. the Workshop on Algorithms and Computational Molecular Biology, International Computer Symposium. Hualien, Taiwan.

Creative Commons License 
Attribution-Noncommercial-Share Alike 2.5 Taiwan License. 
http://creativecommons.org/licenses/by-nc-sa/2.5/tw/
