#ifndef __INET_TCPWESTWOOD_H
#define __INET_TCPWESTWOOD_H

#include "TCPBaseAlg.h"

#include "INETDefs.h"


/**
 * State variables for TCPWestwood.
 */
class INET_API TCPWestwoodStateVariables : public TCPBaseAlgStateVariables
{
  public:
    TCPWestwoodStateVariables();
    virtual std::string info() const;
    virtual std::string detailedInfo() const;
    
	uint32 ssthresh;  ///< slow start threshold

    simtime_t w_RTTmin; // min RTT
	double w_a; // threshold reduction factor for ssthresh calculation
	
	simtime_t w_lastAckTime; // last received ack time
	
	double w_bwe;
	double w_sample_bwe;

	simtime_t* w_sendtime; // each unacked pkt send time is registered
	uint32 w_maxwnd; // max window size for v_sendtime 
	int* w_transmits; // # transmissions of each packet

};


class INET_API TCPWestwood : public TCPBaseAlg
{
protected:
    TCPWestwoodStateVariables *&state; // alias to TCLAlgorithm's 'state'

    /** Create and return a TCPvegasStateVariables object. */
    virtual TCPStateVariables *createStateVariables() {
        return new TCPWestwoodStateVariables();
    }
    
    /** Utility function to recalculate ssthresh */
    virtual void recalculateSlowStartThreshold();

    /** Redefine what should happen on retransmission */
    virtual void processRexmitTimer(TCPEventCode& event);

	/** Recalculate BWE */
    virtual void recalculateBWE(uint32 cumul_ack);

  public:
    /** Ctor */
    TCPWestwood();

    /** Redefine what should happen when data got acked, to add congestion window management */
    virtual void receivedDataAck(uint32 firstSeqAcked);

    /** Redefine what should happen when dupAck was received, to add congestion window management */
    virtual void receivedDuplicateAck();

	/** Called after we send data */
	virtual void dataSent(uint32 fromseq);
};

#endif
