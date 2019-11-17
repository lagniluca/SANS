#include <stdio.h>
#include "cellrepo.h"

void CellRepository::CellRepository::setUnaryOperatorCellStructure(std::string name, CellCategory category, bool signal_changer)
{
    std::vector<std::string> data_inputs{"\\A"};
    std::vector<std::string> selection_input;
    std::vector<std::string> outputs{"\\Y"};

    cells_repository.push_back(new CellStructure(name, category, signal_changer, data_inputs, selection_input, outputs));  
}

void CellRepository::CellRepository::setBinaryOperatorCellStructure(std::string name, CellCategory category, bool signal_changer)
{
    std::vector<std::string> data_inputs{"\\A", "\\B"};
    std::vector<std::string> selection_input;
    std::vector<std::string> outputs{"\\Y"};

    cells_repository.push_back(new CellStructure(name, category, signal_changer, data_inputs, void_vector, outputs));  
}

void CellRepository::CellRepository::setMuxCellStructure(std::string name)
{
    std::vector<std::string> data_inputs{"\\A", "\\B"};
    std::vector<std::string> selection_inputs{"\\S"};
    std::vector<std::string> outputs{"\\Y"};

    cells_repository.push_back(new CellStructure(name, CellCategory::MULTIPLEXER_OPERATOR, false, data_inputs, selection_inputs, outputs));  
}

void CellRepository::CellRepository::buildCellRepository()
{
    std::vector<std::string> nullvect;
    this->setUnaryOperatorCellStructure("$pos", CellCategory::ARITHMETIC_OPERATOR, true);
    this->setUnaryOperatorCellStructure("$neg", CellCategory::ARITHMETIC_OPERATOR, true);

    this->setUnaryOperatorCellStructure("$reduce_and", CellCategory::REDUCTION_OPERATOR, true); 
    this->setUnaryOperatorCellStructure("$reduce_or", CellCategory::REDUCTION_OPERATOR, true);
    this->setUnaryOperatorCellStructure("$reduce_bool", CellCategory::REDUCTION_OPERATOR, true);
    this->setUnaryOperatorCellStructure("$reduce_xor", CellCategory::REDUCTION_OPERATOR, true);
    this->setUnaryOperatorCellStructure("$reduce_xnor", CellCategory::REDUCTION_OPERATOR, true);

    this->setUnaryOperatorCellStructure("$not",   CellCategory::BITWISE_OPERATOR,      true);
    this->setBinaryOperatorCellStructure("$and",  CellCategory::BITWISE_OPERATOR,      true);
    this->setBinaryOperatorCellStructure("$or",   CellCategory::BITWISE_OPERATOR,      true);
    this->setBinaryOperatorCellStructure("$xor",  CellCategory::BITWISE_OPERATOR,      true);
    this->setBinaryOperatorCellStructure("$xnor", CellCategory::BITWISE_OPERATOR,      true);
    
    this->setBinaryOperatorCellStructure("$shl", CellCategory::SHIFT_OPERATOR,         true);
    this->setBinaryOperatorCellStructure("$shr", CellCategory::SHIFT_OPERATOR,         true);
    this->setBinaryOperatorCellStructure("$sshl", CellCategory::SHIFT_OPERATOR,        true);
    this->setBinaryOperatorCellStructure("$sshr", CellCategory::SHIFT_OPERATOR,        true);
    this->setBinaryOperatorCellStructure("$shift", CellCategory::SHIFT_OPERATOR,       true);
    this->setBinaryOperatorCellStructure("$shiftx", CellCategory::SHIFT_OPERATOR,      true);

    this->setBinaryOperatorCellStructure("$lt", CellCategory::COMPARISON_OPERATOR,     true);
    this->setBinaryOperatorCellStructure("$le", CellCategory::COMPARISON_OPERATOR,     true);
    this->setBinaryOperatorCellStructure("$eq", CellCategory::COMPARISON_OPERATOR,     true);
    this->setBinaryOperatorCellStructure("$ne", CellCategory::COMPARISON_OPERATOR,     true);
    this->setBinaryOperatorCellStructure("$eqx", CellCategory::COMPARISON_OPERATOR,    true);
    this->setBinaryOperatorCellStructure("$nex", CellCategory::COMPARISON_OPERATOR,    true);
    this->setBinaryOperatorCellStructure("$ge", CellCategory::COMPARISON_OPERATOR,     true); 
    this->setBinaryOperatorCellStructure("$gt", CellCategory::COMPARISON_OPERATOR,     true); 

    this->setBinaryOperatorCellStructure("$add", CellCategory::ARITHMETIC_OPERATOR,  true);
    this->setBinaryOperatorCellStructure("$sub", CellCategory::ARITHMETIC_OPERATOR,  true);
    this->setBinaryOperatorCellStructure("$mul", CellCategory::ARITHMETIC_OPERATOR,  true);
    this->setBinaryOperatorCellStructure("$div", CellCategory::ARITHMETIC_OPERATOR,  true);
    this->setBinaryOperatorCellStructure("$mod", CellCategory::ARITHMETIC_OPERATOR,  true);
    this->setBinaryOperatorCellStructure("$pow", CellCategory::ARITHMETIC_OPERATOR,  true);

    this->setBinaryOperatorCellStructure("$logic_and", CellCategory::LOGICAL_OPERATOR, true);
    this->setBinaryOperatorCellStructure("$logic_or", CellCategory::LOGICAL_OPERATOR,  true);
    this->setUnaryOperatorCellStructure("$logic_not", CellCategory::LOGICAL_OPERATOR,  true);

    this->setUnaryOperatorCellStructure("$slice", CellCategory::DEDICATED_OPERATOR,    true);
    this->setBinaryOperatorCellStructure("$concat", CellCategory::DEDICATED_OPERATOR,  true);

    this->setBinaryOperatorCellStructure("$macc", CellCategory::COMPARISON_OPERATOR,   true); 
    this->setBinaryOperatorCellStructure("$equiv", CellCategory::COMPARISON_OPERATOR,  true); 

    this->setUnaryOperatorCellStructure("$lut", CellCategory::DEDICATED_OPERATOR,      true);
    this->setUnaryOperatorCellStructure("$sop", CellCategory::LOGICAL_OPERATOR,        true);

    this->setMuxCellStructure("$mux");
    this->setMuxCellStructure("$pmux"); 

    std::vector<std::string> data_inputs_1{"\\P", "\\G", "\\CI"};
    std::vector<std::string> outputs_1{"\\CO"};
    cells_repository.push_back(new CellStructure("$lcu", CellCategory::DEDICATED_OPERATOR, true, data_inputs_1, void_vector, outputs_1));  

    std::vector<std::string> data_inputs_2{"\\A", "\\B", "\\CI", "\\BI"};
    std::vector<std::string> outputs_2{"\\X", "\\Y","\\CO"};
    cells_repository.push_back(new CellStructure("$alu", CellCategory::DEDICATED_OPERATOR, true, data_inputs_2, void_vector, outputs_2));  

    std::vector<std::string> data_inputs_3{"\\A", "\\B", "\\C"};
    std::vector<std::string> outputs_3{"\\X", "\\Y"};
    cells_repository.push_back(new CellStructure("$fa", CellCategory::DEDICATED_OPERATOR,true, data_inputs_3, void_vector, outputs_3));  


    std::vector<std::string> data_inputs_4{"\\P", "\\G", "\\CI"};
    std::vector<std::string> outputs_4{"\\CO"};
    cells_repository.push_back(new CellStructure("$tribuf", CellCategory::RETENTION_OPERATOR, false, data_inputs_4, void_vector, outputs_4));  

    std::vector<std::string> data_inputs_5{"\\SET"};
    std::vector<std::string> selection_inputs_5{"\\CLR"};
    std::vector<std::string> outputs_5{"\\Y"};
    cells_repository.push_back(new CellStructure("$sr", CellCategory::RETENTION_OPERATOR, false, data_inputs_5, selection_inputs_5, outputs_5));

    std::vector<std::string> data_inputs_6{"\\D"};
    std::vector<std::string> outputs_6{"\\Q"};
    cells_repository.push_back(new CellStructure("$ff", CellCategory::RETENTION_OPERATOR, false, data_inputs_6, void_vector, outputs_6));  

    std::vector<std::string> data_inputs_7{"\\D"};
    std::vector<std::string> selection_inputs_7{"\\CLK"};
    std::vector<std::string> outputs_7{"\\Q"};
    cells_repository.push_back(new CellStructure("$dff", CellCategory::RETENTION_OPERATOR, false, data_inputs_7, selection_inputs_7, outputs_7));

    std::vector<std::string> data_inputs_8{"\\D"};
    std::vector<std::string> selection_inputs_8{"\\CLK", "\\EN"};
    std::vector<std::string> outputs_8{"\\Q"};
    cells_repository.push_back(new CellStructure("$dffe", CellCategory::RETENTION_OPERATOR, false, data_inputs_8, selection_inputs_8, outputs_8));

    std::vector<std::string> data_inputs_9{"\\D"};
    std::vector<std::string> selection_inputs_9{"\\CLK", "\\CLR", "\\SET"};
    std::vector<std::string> outputs_9{"\\Q"};
    cells_repository.push_back(new CellStructure("$dffsr", CellCategory::RETENTION_OPERATOR, false, data_inputs_9, selection_inputs_9, outputs_9));

    std::vector<std::string> data_inputs_10{"\\D"};
    std::vector<std::string> selection_inputs_10{"\\CLK", "\\ARST"};
    std::vector<std::string> outputs_10{"\\Q"};
    cells_repository.push_back(new CellStructure("$adff", CellCategory::RETENTION_OPERATOR, false, data_inputs_10, selection_inputs_10, outputs_10));

    std::vector<std::string> data_inputs_11{"\\D"};
    std::vector<std::string> selection_inputs_11{"\\EN"};
    std::vector<std::string> outputs_11{"\\Q"};
    cells_repository.push_back(new CellStructure("$dlatch", CellCategory::RETENTION_OPERATOR, false, data_inputs_11, selection_inputs_11, outputs_11));

    std::vector<std::string> data_inputs_12{"\\D"};
    std::vector<std::string> selection_inputs_12{"\\EN", "\\SET", "\\CLR"};
    std::vector<std::string> outputs_12{"\\Q"};
    cells_repository.push_back(new CellStructure("$dlatchsr", CellCategory::RETENTION_OPERATOR, false, data_inputs_12, selection_inputs_12, outputs_12));

    std::vector<std::string> data_inputs_13{"\\ADDR"};
    std::vector<std::string> selection_inputs_13{"\\EN", "\\CLK"};
    std::vector<std::string> outputs_13{"\\DATA"};
    cells_repository.push_back(new CellStructure("$memrd", CellCategory::RETENTION_OPERATOR, false, data_inputs_13, selection_inputs_13, outputs_13));

    std::vector<std::string> data_inputs_14{"\\RD_ADDR", "\\WR_ADDR", "\\WR_DATA"};
    std::vector<std::string> selection_inputs_14{"\\RD_CLK", "\\RD_EN", "\\WR_CLK", "\\WR_EN"};
    std::vector<std::string> outputs_14{"\\RD_DATA"};
    cells_repository.push_back(new CellStructure("$mem", CellCategory::RETENTION_OPERATOR, false, data_inputs_14, selection_inputs_14, outputs_14));

    std::vector<std::string> data_inputs_15{"\\CTRL_IN"};
    std::vector<std::string> selection_inputs_15{"\\CLK", "\\ARST"};
    std::vector<std::string> outputs_15{"\\CTRL_OUT"};
    cells_repository.push_back(new CellStructure("$fsm", CellCategory::DEDICATED_OPERATOR, false, data_inputs_15, selection_inputs_15, outputs_15));
    
    std::vector<std::string> data_inputs_16{"\\ADDR"};
    std::vector<std::string> selection_inputs_16{"\\EN", "\\CLK" };
    std::vector<std::string> outputs_16 = {"\\DATA"};
    cells_repository.push_back(new CellStructure("$memwr", CellCategory::RETENTION_OPERATOR, false, data_inputs_16, selection_inputs_16, outputs_16));
}

CellRepository::CellRepository::CellRepository()
{
	this->buildCellRepository();
}



CellRepository::CellStructure* CellRepository::CellRepository::getCell(std::string cellType)
{
    CellStructure* res = nullptr;
    
    for(auto cs : this->getCellRepository())
    {
        if(cs->getType().compare(cellType) == 0)
        {
            res = cs;
            break;
        }
    }

    return res;
}
