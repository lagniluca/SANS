/*
 *  General class of signals. There will be two categories of signals, depending on the -condense cmd option value of
 *  the execution where the XML was generated.
 */
public abstract class Signal
{
    //Signal name
    private String name;

    //Constructors
    public Signal() {}
    public Signal (String name) { this.name = name; }

    //toString interface
    protected String sigtostr() { return new String("Signal " + name); }

    //Getters & Setters
    public String getName() { return this.name; }
    public void setName(String name) { this.name = name; }
}