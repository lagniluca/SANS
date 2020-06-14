# Signal Analyzer And Stuff - SANS

The project aims at analyzing input signals of a given Verilog specification. When a signal enters a module, namely for configuration or communication purposes, it can be classified in four general categories:

*   **None**        :   the signal passes in the module but has no effect. This is a result of a no-optimization scenarios;
*   **Selection**   :   the signal enters a component only selection-wise, namely like a S of a MUX;
*   **Data**        :   the signal enters a component through the data ports, namely like A or B of a MUX;
*   **Both**        :   the signal belongs to both categories;

The basic idea we've based our work upon is a construction of an hypergraph statring from the Verilog code.
At hyper-level each node represents a Verilog module whose connections to other modules mirror the high-level configuration of the circuit.
Inside each node we find another graph, this time nodes represent components like simple gates.

By running a DFS algorithm we're able to analyze each path a signal can take from its beginning. Every time the signal enters a selection port, we mark it as selection. Same reasoning undergoes data marking.
After the analysis has been performed, we convert the result in a readable format. By specifying options with command line, various files may be generated, including a .txt, .xml (you can also find a SAX parser in Java if you need to use the XML result), .json and .csv.


If you're ready to dig in the project, follow this:

*   [**Documentation**](Documentation)   :   this directory contains our FULL documentation on our project, including classes and methods explanations. You can also find some useful links to external resources
*   [**SANS**](SANS)            :   the actual code. Instructions on how to use it can be either found in the documentation or in the .sh example in SystemVerilog directory
*   [**Verilog**](Verilog)         :   directory containing all the samples we've tested during development. Some have been developed by us, some belongs to OpenCores - https://opencores.org/

[comment]: <> (Members of this project are Lorenzo Cugini and Luca Lagni.
This is only a port of the original project which is located on GitLab (I've decided to duplicate such project here in order to have all my projects here on github).
Original link: https://gitlab.com/lorenzocugini/mpu)
