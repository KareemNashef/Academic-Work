package OOP.Solution;

import OOP.Provided.HungryStudent;
import OOP.Provided.Restaurant;
import java.util.*;

public class RestaurantImpl implements Restaurant {

    // Restaurant variables
    int resID;
    String resName;
    int resDistance;
    Set<String> resMenu;
    Map<HungryStudent, Integer> resRating;

    public RestaurantImpl(int id, String name, int distance, Set<String> menu) {
        // We assume that the input is valid, we don't check it
        this.resID = id;
        this.resName = name;
        this.resDistance = distance;
        this.resMenu = menu;
        this.resRating = new HashMap<>();
    }

    @Override
    public int distance() {
        // Returning the distance variable
        return this.resDistance;
    }

    @Override
    public Restaurant rate(HungryStudent student, int rating) throws RateRangeException {
        // Checking the validity of the rating
        if (rating < 0 || rating > 5) throw new RateRangeException();

        // Map.put adds/replaces a key's value
        this.resRating.put(student, rating);
        
        return this;
    }

    @Override
    public int numberOfRates() {
        // Returning the size of the ratings array
        return this.resRating.size();
    }

    @Override
    public double averageRating() {
        // Checking the size of the array
        if (this.resRating.isEmpty()) return 0;

        // Getting the sum of all ratings
        double sum = 0;
        for (Map.Entry<HungryStudent, Integer> entry : this.resRating.entrySet()) {
            sum += entry.getValue();
        }

        // Returning the average
        return (sum / this.resRating.size());
    }

    @Override
    public boolean equals(Object inRes) {
        // Checking if the object is the correct type
        if (!(inRes instanceof Restaurant)) return false;

        // Otherwise checking the IDs
        return this.resID == ((RestaurantImpl) inRes).resID;
    }

    @Override
    public int hashCode() {
        return Objects.hash(this.resID);
    }

    @Override
    public String toString() {
        String resData = "";

        // Following the format
        resData = resData.concat("Restaurant: " + this.resName + ".\n");
        resData = resData.concat("Id: " + this.resID + ".\n");
        resData = resData.concat("Distance: " + this.resDistance + ".\n");
        resData = resData.concat("Menu: ");

        // Convert the set to a list
        List<String> sortedMenu = new ArrayList<>(this.resMenu);

        // Sort the list
        Collections.sort(sortedMenu);

        // Iterating over the menu items
        for (int i = 0; i < sortedMenu.size(); i++) {
            resData = resData.concat(sortedMenu.get(i));
            resData = i != sortedMenu.size() - 1 ? resData.concat(", ") : resData.concat(".");
        }

        return resData;
    }

    @Override
    public int compareTo(Restaurant inRes) {
        // Comparing the IDs
        return Integer.compare(this.resID, ((RestaurantImpl) inRes).resID);
    }

    public boolean ratedBy(HungryStudent student) {
        // Check if the student has rated the restaurant
        return this.resRating.containsKey(student);
    }
}
