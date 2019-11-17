import java.util.ArrayList;

public abstract class Parser
{
    //XML file to parse
    protected String filename;

    //Actual modules array
    protected ArrayList<Module> modules = new ArrayList<Module>();

    //Support for unfinished variables
    protected String tmp_str;
    protected Whole_Signal tmp_wsig;
    protected Condensed_Signal tmp_csig;
    protected Module tmp_mod;

    public abstract void parse() throws Exception;

    //Getters & Setters
    public ArrayList<Module> getModules() { return this.modules; }
}