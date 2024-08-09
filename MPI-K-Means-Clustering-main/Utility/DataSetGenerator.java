import java.io.*;
import java.util.*;

public class Main {
    public static void main(String[] args) {
        int M = 1500;
        int N = 10;
        try {
            FileWriter fileWriter = new FileWriter("src\\DataSet"+M+"x"+N+".txt");
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    fileWriter.write(String.valueOf(new Random().nextInt(10)));
                    if (j != N-1) {
                        fileWriter.write(",");
                    }
                }
                if(i!=M-1) {
                    fileWriter.write("\n");
                }
            }
            fileWriter.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
