// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#ifndef GA_H
#define GA_H

#include <math.h>
#include <stdio.h>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
#define GA_BOUND_CHECK 0  // 0 = clamping, 1 = mirroring
#define MAX_NUM_PARALLEL_EVALS 8


//----------------------------------------------------------------------------------------------------------------------
// holds data for typical usage of a GA
struct GAData
{
  int steps;
  int trial;
  float totalFitness;

  GAData() : steps(0), trial(0), totalFitness(0) {};
  GAData(int initSteps) : steps(initSteps), trial(0), totalFitness(0) {};
};

class GeneConverter
{
public:
  virtual ~GeneConverter() {}
  /// Override this to convert from normalised to real values. Gene is the gene number, so gene < genLength.
  virtual float ConvertToRealValue(float normalisedValue, int /*gene*/) const {return normalisedValue;}
  /// Override this to convert a real value into a normalised one. Gene is the gene number, so gene < genLength.
  virtual float ConvertToNormalisedValue(float value, int /*gene*/) const {return value;}
};


/**********************************************************************************
* Microbial GA with Demes
* for details see http://www.cogs.susx.ac.uk/users/inmanh/MicrobialGA_ECAL2009.pdf
***********************************************************************************/
class GA
{
public:

  GA(int popsize = 100, int genlength = 10, int demeWidth = 3);
  ~GA();

  //----------------------------------------------------------------------------------------------------------------------
  // Interface for repeated evaluation of individual genomes

  /// \brief Returns pointer to one of two genomes in current tournament, but doesn't allow modification of its contents
  const float* getCurrentGenome() const;

  /// \brief Sets the fitness of the current genome and selects a new current genome.
  ///  If this is the second genome, finish up tournament and start next one.
  void setFitness(float fit);

  //----------------------------------------------------------------------------------------------------------------------
  // Interface for evaluating N pairs of genomes in parallel
  
  /// \brief Returns 2*N pointers to genomes in the population
  const std::vector<const float*> getNTournamentPairs(int n);
  void setNFitnessPairs(const std::vector<float>& fitnesses);
  
  int getGenomeSize() const { return m_genomeLength; };
  int getCurrentGeneration () const { return m_generation;};

  /// \brief Returns the best genome found so far and its fitness
  const float* getBestGenome(float &fitness) const;

  /// \brief Returns a genome that is the fitness-weighted average of the (evaluated) population
  const float* getWeightedAverageGenome(float &fitness) const;

  void setRandomSeed(long seed) { m_idum = seed; };
  void setDemeWidth(int width) { m_demeWidth = width; };
  void setMutationMax(float mut) { m_maxMutation = mut; };
  void setRecombinationRate(float rcr) { m_recombinationRate = rcr; };
  
  /// \brief Load a previously saved population
  bool loadPopulation(const char* fnm, const GeneConverter* geneConverter = 0);
  /// \brief Save a population to a specified file
  bool savePopulation(const char* fnm, const GeneConverter* geneConverter = 0) const;

  /// \brief Allow manual setting of the genomes in the population
  void setGenome(int iGenome, const float *genome, float fitness);

protected:

  // to store which of the two genomes in the current tournament is being evaluated
  enum GA_STATE
  {
    GA_NONE_SELECTED = 0,
    GA_FIRST_GENOME,
    GA_SECOND_GENOME
  } m_currentGenome;

  // allocate and initialize genome array
  void init();
  
  //----------------------------------------------------------------------------------------------------------------------
  // internal interface for repeated evaluation of individual genomes

  // Generates next couple of genomes to evaluate (randomly from neighborhood in 1d population)
  void startNextTournament();
  // decide winner and loser and perform mutation
  void finishThisTournament();

  // determines winner and loser of tournament and mutates loser
  void performTournament(int indA, float fitA, int indB, float fitB);

  // mutate loser with features from winner (genotype must stay within [0,1] boundaries)
  // loser of a tournament is being replaced by mutated version of the winner
  void mutate(int winner, int loser);

  // returns indices for a random pair of genomes from a neighbourhood defined by demeWidth
  void getRandomPairInDeme(int& indA, int& indB);

  // returns indices for a random pair of genomes that is different from a list of already existing pairs
  void getDifferentRandomPair(int& indA, int& indB, int* existingPairs, int numExistingPairs);

  // checks whether a pair of genomes is different from a list of already existing pairs
  bool pairIsDifferentFrom(int& indA, int& indB, int* existingPairs, int numExistingPairs);
  
  // print out result of evolution to file
  void printFitness(const char* fnm) const;
  void printBestGenome(const char* fnm, const GeneConverter* geneConverter = 0) const;

  // random number generators
  double ran1(long *idum);
  double gasdev(long *idum);

  // the population of genomes: array of arrays of type SCALAR (float/double)
  float** m_genomes;
  // the fitness of each genome (may not be calculated yet)
  float *m_fitnesses;
  // The weighted average genome - only filled in when it's requested
  float *m_weightedAverageGenome;
  
  // stores indices of pairs of requested genomes for parallel evaluations
  int m_requestedGenomes[2 * MAX_NUM_PARALLEL_EVALS];

  int m_popSize;
  int m_genomeLength;
  int m_demeWidth;    // width of 1d neighborhood
  int m_generation;   // current generation
  int m_tournament;   // current tournament (number of evaluations so far)
  int m_currentIndA;  // genomes in current tournament
  int m_currentIndB;

  long m_idum;        // seed for random number generator

  float m_maxMutation;        // amount of Gaussian vector mutation
  float m_recombinationRate;  // amount of Gaussian vector mutation

  float m_fitnessA;   // fitness of first individual in current tournament
  float m_fitnessB;   // fitness of second individual in current tournament
  float m_fitnessAvg; // average in current generation

  static GeneConverter m_defaultGeneConverter;
};

#endif // GA_H
