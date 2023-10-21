import java.util.concurrent.Semaphore;

public class TrafficController{

    private Semaphore sem = new Semaphore(1);

    public void enterLeft() throws InterruptedException{
        sem.acquire();
    }
    public void enterRight() throws InterruptedException{
        sem.acquire();
    }
    public void leaveLeft() {
        sem.release();
    }
    public void leaveRight() {
        sem.release();
    }

}