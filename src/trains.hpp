#ifndef TICKETSYSTEM_TRAINS_HPP_
#define TICKETSYSTEM_TRAINS_HPP_

#include "../lib/bplustree.hpp"
#include "../lib/utils.hpp"

class Train {
  bool is_release;
  FixedStr<20> train_id;
  int station_num, seat_num;
  FixedStr<30> stations[101];
  int prices[101], travel_times[101], stopover_times[101];
  char type;

 public:
  
};

class TrainManagement {
  BPlusTree<FixedStr<20>, int, Train> trains;

 public:
  TrainManagement() : trains{"trains_index.bin", "trains.bin"} {}
  bool AddTrain() {

  }
};

#endif  // TICKETSYSTEM_TRAINS_HPP_