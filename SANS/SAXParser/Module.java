import java.util.ArrayList;

/*
 *  Module container
 */
public class Module
{
    //Module name
    private String moduleName;
    //Signals of the module
    private ArrayList<Signal> signals;

    //Constructors
    public Module() {
        signals = new ArrayList<Signal>();
    }

    public void addSignal(Signal sig) { this.signals.add(sig); }

    //toString interface
    String modtostr()
    {
        String tmp = "";

        tmp += "Module name:\t" + this.moduleName + "\n";

        for( Signal sig : signals )
        {
            tmp += sig.sigtostr() + "\n";
        }

        tmp += "\n";

        return tmp;
    }

    //Getters & Setters
    public void setModuleName(String moduleName)
    {
        this.moduleName = moduleName;
    }
}