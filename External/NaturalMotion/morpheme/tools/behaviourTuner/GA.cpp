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
#include "GA.h"
#include <time.h>
#include <utility>
#include "assert.h"

#ifdef WIN32
# pragma warning( disable : 4996 ) 
#endif

GeneConverter GA::m_defaultGeneConverter;

//----------------------------------------------------------------------------------------------------------------------
GA::GA(int popsize, int genomeLength, int demeWidth) : 
  m_popSize(popsize), 
  m_genomeLength(genomeLength),
  m_demeWidth(demeWidth)
{
  init();
};

//----------------------------------------------------------------------------------------------------------------------
GA::~GA()
{
  for(int i = 0; i < m_popSize; ++i)
    delete [] m_genomes[i];
  delete [] m_genomes;
  delete [] m_fitnesses;
  delete [] m_weightedAverageGenome;
}

//----------------------------------------------------------------------------------------------------------------------
void GA::init()
{
  // some defaults
  m_maxMutation = 0.15f;      // amount of mutation  (scales a gaussian random variable)
  m_recombinationRate = 0.5f; // probability of copying a gene from winner to loser of tournament

  // initialization
  m_fitnessA = 0.0f;
  m_fitnessB = 0.0f;
  m_fitnessAvg = 0.0f;
  m_tournament = 0;
  m_generation = 0;
  m_idum = (long)-time(0); // seed random number generator
  m_currentGenome = GA_NONE_SELECTED;
  
  // allocate 
  m_genomes = new float*[m_popSize];
  for(int i=0; i<m_popSize; i++)
    m_genomes[i] = new float[m_popSize];

  // random initialization
  for (int i=0; i < m_popSize; i++)
    for (int j=0; j < m_genomeLength; j++)
      m_genomes[i][j] = (float)ran1(&m_idum);

  m_fitnesses = new float[m_popSize];
  for(int i=0; i<m_popSize; i++)
    m_fitnesses[i] = -1.0f;

  m_weightedAverageGenome = new float[m_popSize];
  
  // select first pair of genomes for tournament
  startNextTournament();
}

//----------------------------------------------------------------------------------------------------------------------
const float* GA::getCurrentGenome() const
{
  if(m_currentGenome == GA_FIRST_GENOME)
    return m_genomes[m_currentIndA];
  else 
    return m_genomes[m_currentIndB];
}

//----------------------------------------------------------------------------------------------------------------------
const float* GA::getBestGenome(float &fitness) const
{
  fitness = -1.0f;
  float *result = 0;
  for (int i = 0 ; i < m_popSize ; ++i)
  {
    if (m_fitnesses[i] >= fitness)
    {
      fitness = m_fitnesses[i];
      result = m_genomes[i];
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const float* GA::getWeightedAverageGenome(float &fitness) const
{
  fitness = 0.0f;
  for (int i = 0 ; i < m_genomeLength ; ++i)
    m_weightedAverageGenome[i] = 0.0f;

  float totalFitness = 0.0f;
  for (int i = 0 ; i < m_popSize ; ++i)
  {
    if (m_fitnesses[i] > 0.0f)
    {
      totalFitness += m_fitnesses[i];
      fitness += m_fitnesses[i] * m_fitnesses[i];
      for (int j = 0 ; j < m_genomeLength ; ++j)
        m_weightedAverageGenome[j] += m_genomes[i][j] * m_fitnesses[i];
    }
  }
  if (totalFitness > 0.0f)
  {
    fitness /= totalFitness;
    for (int i = 0 ; i < m_genomeLength ; ++i)
      m_weightedAverageGenome[i] /= totalFitness;
  }
  return m_weightedAverageGenome;
}

//----------------------------------------------------------------------------------------------------------------------
void GA::setFitness(float fit)
{
  // If 1st genome: simply cache fitness
  if(m_currentGenome == GA_FIRST_GENOME)
  {
    m_fitnessA = fit;
    m_currentGenome = GA_SECOND_GENOME;
    m_fitnesses[m_currentIndA] = fit;
  }
  // If 2nd genome, finish tournament also
  else if(m_currentGenome == GA_SECOND_GENOME)
  {
    m_fitnessB = fit;
    m_fitnesses[m_currentIndB] = fit;
    finishThisTournament();
    startNextTournament();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void GA::getRandomPairInDeme(int& indA, int& indB)
{
  indA = (int)(m_popSize*ran1(&m_idum));
  indB = indA;
  while (indB == indA)
  {
    double r = ran1(&m_idum);
    indB = indA + (int) (m_demeWidth * r);
    indB = (indB + m_popSize) % m_popSize;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void GA::getDifferentRandomPair(int& indA, int& indB, int* existingPairs, int numPairs)
{
  do
  {
    getRandomPairInDeme(indA, indB);
  } while(!pairIsDifferentFrom(indA, indB, existingPairs, numPairs));
}

//----------------------------------------------------------------------------------------------------------------------
bool GA::pairIsDifferentFrom(int& indA, int& indB, int* existingPairs, int numExistingPairs)
{
  bool isDifferent = true;
  for(int i=0; i<numExistingPairs; i++)
  {
    int otherA = existingPairs[i*2];
    int otherB = existingPairs[i*2+1];
    if ( indA == otherA || indA == otherB || indB == otherA || indB == otherB )
    {
      isDifferent = false;
      break;
    }
  }
  return isDifferent;
}

//----------------------------------------------------------------------------------------------------------------------
void GA::startNextTournament()
{
  getRandomPairInDeme(m_currentIndA, m_currentIndB);
  m_currentGenome = GA_FIRST_GENOME;
}

//----------------------------------------------------------------------------------------------------------------------
void GA::finishThisTournament()
{
  performTournament(m_currentIndA, m_fitnessA, m_currentIndB, m_fitnessB);
}

//----------------------------------------------------------------------------------------------------------------------
void GA::performTournament(int indA, float fitA, int indB, float fitB)
{
  int winner, loser;
  if (fitA > fitB)
  {
    winner = indA;
    loser = indB;
    m_fitnessAvg += fitA/m_popSize;
    m_fitnesses[winner] = fitA;
  }
  else 
  {
    winner = indB;
    loser = indA;
    m_fitnessAvg += fitB/m_popSize;
    m_fitnesses[winner] = fitB;
  }

  // do mutation
  mutate(winner, loser);

  m_fitnesses[loser] = -1.0f;

  m_tournament++;

  // reset at end of generation
  if ((m_tournament+1) % m_popSize == 0)
  {
    m_generation++;
    m_fitnessAvg = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
const std::vector<const float*> GA::getNTournamentPairs(int n)
{
  assert((2*n <= m_popSize/2) && (n < MAX_NUM_PARALLEL_EVALS)); 
  
  // 1st pair randomly:
  getRandomPairInDeme(m_requestedGenomes[0], m_requestedGenomes[1]);
 
  // remaining pairs must be different:
  for(int i=1; i<n; i++)
  {
    getDifferentRandomPair(m_requestedGenomes[2*i], m_requestedGenomes[2*i+1], &m_requestedGenomes[0], i);
  }

  // collect pointers
  std::vector<const float*> tournamentPairs;
  for(int i=0; i<n*2; i++)
    tournamentPairs.push_back(m_genomes[m_requestedGenomes[i]]);
  return tournamentPairs;
}

//----------------------------------------------------------------------------------------------------------------------
void GA::setNFitnessPairs(const std::vector<float>& fitnesses)
{
  assert((fitnesses.size() < 2*MAX_NUM_PARALLEL_EVALS) && (fitnesses.size() % 2 == 0));

  // use fitness to perform tournament/mutation
  for (int i=0; i<(int) fitnesses.size()/2; i++)
  {
    int indA = 2*i;
    int indB = 2*i+1;
    performTournament(m_requestedGenomes[indA], fitnesses[indA], m_requestedGenomes[indB], fitnesses[indB]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void GA::mutate(int winner, int loser)
{
  for (int i=0; i < m_genomeLength; i++)
  {      
    if (ran1(&m_idum) < m_recombinationRate) 
    {
      // recombination/genetic infection (some genes are copied from winner to loser)
      m_genomes[loser][i] = m_genomes[winner][i];
    }
    else
    {
      // gaussian mutation
      m_genomes[loser][i] += (float) (gasdev(&m_idum) * m_maxMutation);
    }

    // ensure values stay in [0,1] range
    if (m_genomes[loser][i] > 1)
    {
#if GA_BOUND_CHECK == 0
      m_genomes[loser][i] = 1.f; // clamp
#else
      m_genomes[loser][i] = 2.f - m_genomes[loser][i]; // mirror
#endif
    }
    else if (m_genomes[loser][i] < 0)
    {
#if GA_BOUND_CHECK == 0
      m_genomes[loser][i] =  0.f;
#else
      m_genomes[loser][i] *=  -1.f;
#endif
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void GA::setGenome(int iGenome, const float *genome, float fitness)
{
  for (int j=0; j < m_genomeLength; j++)
  {
    m_genomes[iGenome][j] = genome[j];
  }
  m_fitnesses[iGenome] = fitness;
}

static const int VERSION=1;

//----------------------------------------------------------------------------------------------------------------------
bool GA::loadPopulation(const char* fnm, const GeneConverter* geneConverter)
{
  const GeneConverter& gc = geneConverter ? *geneConverter : m_defaultGeneConverter;

  FILE* fp = fopen(fnm, "r");
  if (fp)
  {
    int version = -1;
    fscanf(fp, "Version %d\n", &version);
    if (version != VERSION)
    {
      fprintf(stderr, "Error: Attempt to load a population with version %d instead of %d\n", version, VERSION);
      fclose(fp);
      return false;
    }

    // sanity check some things that must be the same
    int genomeLength = -1;
    fscanf(fp, "GenomeLength %d\n", &genomeLength);
    if (genomeLength != m_genomeLength)
    {
      fprintf(stderr, "Error: Attempt to load a genome length (%d) that is different to that specified in the configuration\n",
        genomeLength, m_genomeLength);
      fclose(fp);
      return false;
    }

    int popSize = -1;
    fscanf(fp, "PopulationSize %d\n", &popSize);
    if (popSize != m_popSize)
    {
      fprintf(stderr, "Warning: Attempt to load a different population size (%d) to that specified in the configuration (%d)\n",
        popSize, m_popSize);
    }

    // overwrite internal settings
    fscanf(fp, "DemeWidth %d\n", &m_demeWidth);
    fscanf(fp, "Generation %d\n", &m_generation);
    fscanf(fp, "Tournament %d\n", &m_tournament);
    fscanf(fp, "CurrentIndA %d\n", &m_currentIndA);
    fscanf(fp, "CurrentIndB %d\n", &m_currentIndB);

    fscanf(fp, "MaxMutation %g\n", &m_maxMutation);
    fscanf(fp, "RecombinationRate %g\n", &m_recombinationRate);
    fscanf(fp, "FitnessA %g\n", &m_fitnessA);
    fscanf(fp, "FitnessB %g\n", &m_fitnessB);
    fscanf(fp, "FitnessAverage %g\n", &m_fitnessAvg);

    // Fixup in case the population size has decreased
    if (m_currentIndA >= m_popSize)
      m_currentIndA = (m_currentIndB + 1) % m_popSize;
    if (m_currentIndB >= m_popSize)
      m_currentIndB = (m_currentIndA + 1) % m_popSize;

    for (int i=0; i < std::min(popSize, m_popSize); i++)
    {
      float fitness;
      fscanf(fp, "%f\t", &fitness); 
      m_fitnesses[i] = fitness;
      for (int j=0; j < m_genomeLength; j++)
      {
        float tempD;
        fscanf(fp, "%f\t", &tempD); 
        m_genomes[i][j] = gc.ConvertToNormalisedValue(tempD, j);
      }
    }
    fclose(fp);
    return true;
  }
  else
  {
    fprintf(stderr, "Couldn't open population file for reading: %s\n", fnm);
    return false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool GA::savePopulation(const char* fnm, const GeneConverter* geneConverter) const
{
  const GeneConverter& gc = geneConverter ? *geneConverter : m_defaultGeneConverter;

  FILE* fp = fopen(fnm,"w+");
  if (fp)
  {
    fprintf(fp, "Version %d\n", VERSION);
    fprintf(fp, "GenomeLength %d\n", m_genomeLength);
    fprintf(fp, "PopulationSize %d\n", m_popSize);

    // overwrite internal settings
    fprintf(fp, "DemeWidth %d\n", m_demeWidth);
    fprintf(fp, "Generation %d\n", m_generation);
    fprintf(fp, "Tournament %d\n", m_tournament);
    fprintf(fp, "CurrentIndA %d\n", m_currentIndA);
    fprintf(fp, "CurrentIndB %d\n", m_currentIndB);

    fprintf(fp, "MaxMutation %g\n", m_maxMutation);
    fprintf(fp, "RecombinationRate %g\n", m_recombinationRate);
    fprintf(fp, "FitnessA %g\n", m_fitnessA);
    fprintf(fp, "FitnessB %g\n", m_fitnessB);
    fprintf(fp, "FitnessAverage %g\n", m_fitnessAvg);

    for (int i=0; i < m_popSize; i++)
    {
      fprintf(fp, "%f\t", m_fitnesses[i]);
      for (int j=0; j < m_genomeLength; j++)
        fprintf(fp, "%f\t", gc.ConvertToRealValue(m_genomes[i][j], j));
      fprintf(fp, "\n");
    }
    fflush(fp);
    fclose(fp);
    return true;
  }
  else
  {
    fprintf(stderr, "Couldn't open population file for writing: %s\n", fnm);
    return false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void GA::printFitness(const char* fnm) const
{
  float fitnessMax = -1.0f;
  (void) getBestGenome(fitnessMax);

  FILE* fp = fopen(fnm, "a");
  if (fp)
  {
    fprintf(fp,"%i\t%f\t%f\n", m_generation, fitnessMax, m_fitnessAvg);
    fclose(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void GA::printBestGenome(const char* fnm, const GeneConverter* geneConverter) const
{
  const GeneConverter& gc = geneConverter ? *geneConverter : m_defaultGeneConverter;
  // save best agent
  FILE* fp = fopen(fnm, "a");
  if (fp)
  {
    fprintf(fp, "%i\t", m_generation);
    float junk;
    for (int i=0; i < m_genomeLength; i++)
      fprintf(fp, "%f ", gc.ConvertToRealValue(getBestGenome(junk)[i], i));
    fprintf(fp, "\n");
    fclose(fp);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Fast random number generator from "Numerical Recipes in C"
//----------------------------------------------------------------------------------------------------------------------
#define IAA 16807
#define IM 2147483647
#define AM (1. /IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

//----------------------------------------------------------------------------------------------------------------------
double GA::ran1(long *idum)
{
  int j;
  long k;
  static long iy=0;
  static long iv[NTAB];
  double temp;
  if (*idum <= 0 || !iy) 
  {
    if (-(*idum) < 1) 
      *idum = 1;
    else 
      *idum = -(*idum);
    for (j=NTAB+7; j>=0; j--) 
    {
      k = (*idum)/IQ;
      *idum=IAA*(*idum-k*IQ)-IR*k;
      if (*idum <0) 
        *idum += IM;
      if (j < NTAB) 
        iv[j] = *idum;
    }
    iy = iv[0];
  }
  k = (*idum)/IQ;
  *idum=IAA*(*idum-k*IQ)-IR*k;
  if (*idum < 0) 
    *idum += IM;
  j = iy/NDIV;
  iy=iv[j];
  iv[j] = *idum;
  if ((temp=AM*iy) > RNMX) 
    return RNMX;
  else 
    return temp;
}

//----------------------------------------------------------------------------------------------------------------------
double GA::gasdev(long *idum)
{
  static int iset=0;
  static double gset;
  double fac,rsq,v1,v2;
  if (iset == 0) 
  {
    do 
    {
      v1=2.0*ran1(idum)-1.0;
      v2=2.0*ran1(idum)-1.0;
      rsq=v1*v1+v2*v2;
    } while (rsq >= 1.0 || rsq==0.0);
    fac=sqrt(-2.0*log(rsq)/rsq);
    gset=v1*fac;
    iset=1;
    return v2*fac;
  } 
  else 
  {
    iset =0;
    return gset;
  }
}
