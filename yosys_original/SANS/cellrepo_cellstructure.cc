#include <stdio.h>
#include "cellrepo.h"

void CellRepository::CellStructure::setType(std::string type){ this->type = type; }
void CellRepository::CellStructure::setCategory(CellCategory category){ this->category = category; }
void CellRepository::CellStructure::setSignalChanger(bool signal_changer){ this->signal_changer = signal_changer; }
void CellRepository::CellStructure::setDataInputs(std::vector<std::string> data_inputs){ this->data_inputs = data_inputs; }
void CellRepository::CellStructure::setSelectionInputs(std::vector<std::string> selection_inputs){ this->selection_inputs = selection_inputs; }
void CellRepository::CellStructure::setOutputs(std::vector<std::string> outputs){ this->outputs = outputs; }

CellRepository::CellStructure::CellStructure(std::string type, CellCategory category, bool signal_changer, std::vector<std::string> data_inputs,std::vector<std::string> selection_inputs, std::vector<std::string> outputs)
{
    this->setType(type);
    this->setCategory(category);
    this->setSignalChanger(signal_changer);
    this->setDataInputs(data_inputs);
    this->setSelectionInputs(selection_inputs);
    this->setOutputs(outputs);
}

std::string CellRepository::CellStructure::getType(){ return this->type; }
CellRepository::CellCategory CellRepository::CellStructure::getCategory(){ return this-> category; }
bool CellRepository::CellStructure::getSignalChanger(){ return this->signal_changer; }
std::vector<std::string> CellRepository::CellStructure::getDataInputs(){ return this->data_inputs; }
std::vector<std::string> CellRepository::CellStructure::getSelectionInputs(){ return this->selection_inputs; }
std::vector<std::string> CellRepository::CellStructure::getOutputs(){ return this->outputs; }


bool CellRepository::CellStructure::isDataInput(std::string unknown_port)
{
    for(auto p : this->getDataInputs())
	{
		if(p.compare(unknown_port) == 0)
    	{
    		return true;
    	}
    }
    return false ;
}

bool CellRepository::CellStructure::isSelectionInput(std::string unknown_port)
{
    for(auto p : this->getSelectionInputs())
    {
    	if(p.compare(unknown_port) == 0)
    	{
    		return true;
    	}
    }
    return false ;
}

bool CellRepository::CellStructure::isOutput(std::string unknown_port)
{
    for(auto p : this->getOutputs())
    {
    	if(p.compare(unknown_port) == 0)
    	{
    		return true;
    	}
    }
    return false ;
}

bool CellRepository::CellStructure::isLogicalOperator()
{
   	if(this->getCategory() == CellCategory::LOGICAL_OPERATOR)
   	{
   		return true;
   	}
    return false;
}

 bool CellRepository::CellStructure::isArithmeticOperator()
 {
    if(this->getCategory() == CellCategory::ARITHMETIC_OPERATOR)
    {
    	return true;
    }
    return false;
}

bool CellRepository::CellStructure::isBitwiseOperator()
{
    if(this->getCategory() == CellCategory::BITWISE_OPERATOR)
    {
    	return true;
    }
    return false;
}

bool CellRepository::CellStructure::isComparisonOperator()
{
    if(this->getCategory() == CellCategory::COMPARISON_OPERATOR)
    {
    	return true;
    }
    return false;
}

bool CellRepository::CellStructure::isMultiplexerOperator()
{
    if(this->getCategory() == CellCategory::MULTIPLEXER_OPERATOR)
	{
    	return true;
    }
    return false;
}

bool CellRepository::CellStructure::isRetentionOperator(){
    if(this->getCategory() == CellCategory::RETENTION_OPERATOR)
    {
    	return true;
    }
    return false;
}

bool CellRepository::CellStructure::isReductionOperator(){
    if(this->getCategory() == CellCategory::REDUCTION_OPERATOR)
    {
    	return true;
    }
    return false;
}

bool CellRepository::CellStructure::isShiftOperator(){
    if(this->getCategory() == CellCategory::SHIFT_OPERATOR)
    {
    	return true;
    }
    return false;
}

bool CellRepository::CellStructure::isDedicatedOperator(){
    if(this->getCategory() == CellCategory::DEDICATED_OPERATOR)
    {
    	return true;
    }
    return false;
}

std::string CellRepository::CellStructure::toString()
{
    std::stringstream ss;

    ss << "\n==========={ Cell Structure }==========";

    ss << "\nType: " << this->getType();
    ss << "\nSignalChanger :" << this->getSignalChanger();
    ss << "\nData inputs: ";
    for(auto di : this->getDataInputs())
        ss << "" << di << "\t";
    ss << "\nSelection inputs: ";
    for(auto si : this->getSelectionInputs())
        ss << "" << si << "\t";
    ss << "\nOutputs : ";
    for(auto ou : this->getOutputs())
        ss << "" << ou << "\t";

    ss << "\n";
    return ss.str();
}