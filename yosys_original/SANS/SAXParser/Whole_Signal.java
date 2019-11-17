/*
 *  Class generated if the last sans' run wasn't condensed.
 */
public class Whole_Signal extends Signal
{
    private boolean data;
    private boolean select;

    //Constructors
    public Whole_Signal() {}
    public Whole_Signal(String name, boolean data, boolean select)
    {
        super(name);
        this.data = data;
        this.select = select;
    }

    //Getters & Setters
    public void setData(boolean data) { this.data = data; }
    public void setSelect(boolean select) { this.select = select; }

    @Override
    protected String sigtostr()
    {
        return new String("Signal:\t" + super.getName() + "\t: <data " + data + "> <select " + select + ">");
    }
}