/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 UCLA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: 
 */

/**
 * \ingroup ccnx
 * \defgroup CcnxStackModel Ccnx Stack Model
 *
 * \section CcnxStackTracingModel Tracing in the Ccnx Stack
 *
 * The ccnx stack provides a number of trace sources in its various
 * protocol implementations.  These trace sources can be hooked using your own 
 * custom trace code, or you can use our helper functions in some cases to 
 * arrange for tracing to be enabled.
 *
 * \subsection CcnxStackCcnxTracingModel Tracing in Ccnx
 *
 * The Ccnx layer three protocol provides three trace hooks.  These are the 
 * "Tx" (ns3::CcnxL3Protocol::m_txTrace), "Rx" (ns3::CcnxL3Protocol::m_rxTrace) 
 * and "Drop" (ns3::CcnxL3Protocol::m_dropTrace) trace sources.
 *
 * The "Tx" trace is fired in a number of situations, all of which indicate that
 * a given packet is about to be sent down to a given ns3::CcnxFace.
 *
 * - \todo list Tx trace events
 *
 * The "Rx" trace is fired when a packet is passed from the device up to the
 * ns3::CcnxL3Protocol::Receive function.
 *
 * - In the receive function, the CcnxFaceList is iterated, and if the
 *   CcnxFace corresponding to the receiving device is found to be in the
 *   UP state, the trace is fired.
 *
 * The "Drop" trace is fired in any case where the packet is dropped (in both
 * the transmit and receive paths).
 *
 * - \todo list Drop trace events
 */

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/object.h"
#include "ns3/names.h"
#include "ns3/ccnx.h"
#include "ns3/packet-socket-factory.h"
#include "ns3/config.h"
#include "ns3/simulator.h"
#include "ns3/string.h"
#include "ns3/net-device.h"
#include "ns3/callback.h"
#include "ns3/node.h"
#include "ns3/core-config.h"
#include "ns3/ccnx-forwarding-protocol.h"

#include "ccnx-stack-helper.h"
#include "ccnx-forwarding-helper.h"

#include <limits>
#include <map>

NS_LOG_COMPONENT_DEFINE ("CcnxStackHelper");

namespace ns3 {

// Things are going to work differently here with respect to trace
// file handling than in most places because the Tx and Rx trace
// sources we are interested in are going to multiplex receive and
// transmit callbacks for all Ccnx and face pairs through one
// callback.  We want packets to or from each distinct pair to go to
// an individual file, so we have got to demultiplex the Ccnx and face
// pair into a corresponding Ptr<PcapFileWrapper> at the callback.
//
// A complication in this situation is that the trace sources are
// hooked on a protocol basis.  There is no trace source hooked by an
// Ccnx and face pair.  This means that if we naively proceed to hook,
// say, a drop trace for a given Ccnx with face 0, and then hook for
// Ccnx with face 1 we will hook the drop trace twice and get two
// callbacks per event.  What we need to do is to hook the event once,
// and that will result in a single callback per drop event, and the
// trace source will provide the face which we filter on in the trace
// sink.
// 
// This has got to continue to work properly after the helper has been
// destroyed; but must be cleaned up at the end of time to avoid
// leaks.  Global maps of protocol/face pairs to file objects seems to
// fit the bill.
//
typedef std::pair<Ptr<Ccnx>, uint32_t> FacePairCcnx; 
typedef std::map<FacePairCcnx, Ptr<PcapFileWrapper> > FaceFileMapCcnx;
typedef std::map<FacePairCcnx, Ptr<OutputStreamWrapper> > FaceStreamMapCcnx;

static FaceFileMapCcnx g_faceFileMapCcnx; /**< A mapping of Ccnx/face pairs to pcap files */
static FaceStreamMapCcnx g_faceStreamMapCcnx; /**< A mapping of Ccnx/face pairs to ascii streams */

CcnxStackHelper::CcnxStackHelper ()
  : m_forwarding (0)
  , m_ccnxEnabled (true)
{
  Initialize ();
}

// private method called by both constructor and Reset ()
void
CcnxStackHelper::Initialize ()
{
  // CcnxStaticForwardingHelper staticForwarding;
  // CcnxGlobalForwardingHelper globalForwarding;
  // CcnxListForwardingHelper listForwarding;
  // listForwarding.Add (staticForwarding, 0);
  // listForwarding.Add (globalForwarding, -10);
  // SetForwardingHelper (listForwarding);
}

CcnxStackHelper::~CcnxStackHelper ()
{
  if (m_forwarding)
    {
      delete m_forwarding;
      m_forwarding = 0;
    }
}

CcnxStackHelper::CcnxStackHelper (const CcnxStackHelper &o)
{
  m_forwarding = o.m_forwarding->Copy ();
  m_ccnxEnabled = o.m_ccnxEnabled;
}

CcnxStackHelper &
CcnxStackHelper::operator = (const CcnxStackHelper &o)
{
  if (this == &o)
    {
      return *this;
    }
  m_forwarding = o.m_forwarding->Copy ();
  return *this;
}

void
CcnxStackHelper::Reset (void)
{
  delete m_forwarding;
  m_forwarding = 0;
  m_ccnxEnabled = true;
  Initialize ();
}

void 
CcnxStackHelper::SetForwardingHelper (const CcnxForwardingHelper &forwarding)
{
  delete m_forwarding;
  m_forwarding = forwarding.Copy ();
}

void
CcnxStackHelper::SetCcnxStackInstall (bool enable)
{
  m_ccnxEnabled = enable;
}

void 
CcnxStackHelper::Install (NodeContainer c) const
{
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Install (*i);
    }
}

void 
CcnxStackHelper::InstallAll (void) const
{
  Install (NodeContainer::GetGlobal ());
}

void
CcnxStackHelper::CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId)
{
  ObjectFactory factory;
  factory.SetTypeId (typeId);
  Ptr<Object> protocol = factory.Create <Object> ();
  node->AggregateObject (protocol);
}

void
CcnxStackHelper::Install (Ptr<Node> node) const
{
  NS_ASSERT_MSG (m_forwarding, "CcnxForwarding should be set prior calling Install() method");
  
  if (m_ccnxEnabled)
    {
      if (node->GetObject<Ccnx> () != 0)
        {
          NS_FATAL_ERROR ("CcnxStackHelper::Install (): Installing " 
                          "a CcnxStack to a node with an existing Ccnx object");
          return;
        }

      CreateAndAggregateObjectFromTypeId (node, "ns3::CcnxL3Protocol");
      // Set forwarding
      Ptr<Ccnx> ccnx = node->GetObject<Ccnx> ();
      Ptr<CcnxForwardingProtocol> ccnxForwarding = m_forwarding->Create (node);
      ccnx->SetForwardingProtocol (ccnxForwarding);
    }
}

void
CcnxStackHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  Install (node);
}

static void
CcnxL3ProtocolRxTxSink (Ptr<const Packet> p, Ptr<Ccnx> ccnx, uint32_t face)
{
  NS_LOG_FUNCTION (p << ccnx << face);

  //
  // Since trace sources are independent of face, if we hook a source
  // on a particular protocol we will get traces for all of its faces.
  // We need to filter this to only report faces for which the user 
  // has expressed interest.
  //
  FacePairCcnx pair = std::make_pair (ccnx, face);
  if (g_faceFileMapCcnx.find (pair) == g_faceFileMapCcnx.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from face " << face);
      return;
    }

  Ptr<PcapFileWrapper> file = g_faceFileMapCcnx[pair];
  file->Write (Simulator::Now (), p);
}

bool
CcnxStackHelper::PcapHooked (Ptr<Ccnx> ccnx)
{
  for (FaceFileMapCcnx::const_iterator i = g_faceFileMapCcnx.begin (); 
       i != g_faceFileMapCcnx.end (); 
       ++i)
    {
      if ((*i).first.first == ccnx)
        {
          return true;
        }
    }
  return false;
}

void 
CcnxStackHelper::EnablePcapCcnxInternal (std::string prefix, Ptr<Ccnx> ccnx, uint32_t face, bool explicitFilename)
{
  NS_LOG_FUNCTION (prefix << ccnx << face);

  if (!m_ccnxEnabled)
    {
      NS_LOG_INFO ("Call to enable Ccnx pcap tracing but Ccnx not enabled");
      return;
    }

  //
  // We have to create a file and a mapping from protocol/face to file 
  // irrespective of how many times we want to trace a particular protocol.
  //
  PcapHelper pcapHelper;

  std::string filename;
  if (explicitFilename)
    {
      filename = prefix;
    }
  else
    {
      filename = pcapHelper.GetFilenameFromInterfacePair (prefix, ccnx, face);
    }

  Ptr<PcapFileWrapper> file = pcapHelper.CreateFile (filename, std::ios::out, PcapHelper::DLT_RAW);

  //
  // However, we only hook the trace source once to avoid multiple trace sink
  // calls per event (connect is independent of face).
  //
  if (!PcapHooked (ccnx))
    {
      //
      // Ptr<Ccnx> is aggregated to node and CcnxL3Protocol is aggregated to 
      // node so we can get to CcnxL3Protocol through Ccnx.
      //
      Ptr<CcnxL3Protocol> ccnxL3Protocol = ccnx->GetObject<CcnxL3Protocol> ();
      NS_ASSERT_MSG (ccnxL3Protocol, "CcnxStackHelper::EnablePcapCcnxInternal(): "
                     "m_ccnxEnabled and ccnxL3Protocol inconsistent");

      bool result = ccnxL3Protocol->TraceConnectWithoutContext ("Tx", MakeCallback (&CcnxL3ProtocolRxTxSink));
      NS_ASSERT_MSG (result == true, "CcnxStackHelper::EnablePcapCcnxInternal():  "
                     "Unable to connect ccnxL3Protocol \"Tx\"");

      result = ccnxL3Protocol->TraceConnectWithoutContext ("Rx", MakeCallback (&CcnxL3ProtocolRxTxSink));
      NS_ASSERT_MSG (result == true, "CcnxStackHelper::EnablePcapCcnxInternal():  "
                     "Unable to connect ccnxL3Protocol \"Rx\"");
      // cast result to void, to suppress ‘result’ set but not used compiler-warning
      // for optimized builds
      (void) result;
    }

  g_faceFileMapCcnx[std::make_pair (ccnx, face)] = file;
}

static void
CcnxL3ProtocolDropSinkWithoutContext (
  Ptr<OutputStreamWrapper> stream,
  Ptr<const Packet> packet,
  CcnxL3Protocol::DropReason reason, 
  Ptr<Ccnx> ccnx, 
  uint32_t face)
{
  //
  // Since trace sources are independent of face, if we hook a source
  // on a particular protocol we will get traces for all of its faces.
  // We need to filter this to only report faces for which the user 
  // has expressed interest.
  //
  FacePairCcnx pair = std::make_pair (ccnx, face);
  if (g_faceStreamMapCcnx.find (pair) == g_faceStreamMapCcnx.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from face " << face);
      return;
    }

  *stream->GetStream () << "d " << Simulator::Now ().GetSeconds () << " " << *packet << std::endl;
}

static void
CcnxL3ProtocolDropSinkWithContext (
  Ptr<OutputStreamWrapper> stream,
  std::string context,
  Ptr<const Packet> packet,
  CcnxL3Protocol::DropReason reason, 
  Ptr<Ccnx> ccnx, 
  uint32_t face)
{
  //
  // Since trace sources are independent of face, if we hook a source
  // on a particular protocol we will get traces for all of its faces.
  // We need to filter this to only report faces for which the user 
  // has expressed interest.
  //
  FacePairCcnx pair = std::make_pair (ccnx, face);
  if (g_faceStreamMapCcnx.find (pair) == g_faceStreamMapCcnx.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from face " << face);
      return;
    }

  *stream->GetStream () << "d " << Simulator::Now ().GetSeconds () << " " << context << "(" << face << ") " 
                        << *packet << std::endl;
}

bool
CcnxStackHelper::AsciiHooked (Ptr<Ccnx> ccnx)
{
  for (  FaceStreamMapCcnx::const_iterator i = g_faceStreamMapCcnx.begin (); 
         i != g_faceStreamMapCcnx.end (); 
         ++i)
    {
      if ((*i).first.first == ccnx)
        {
          return true;
        }
    }
  return false;
}

void 
CcnxStackHelper::EnableAsciiCcnxInternal (
  Ptr<OutputStreamWrapper> stream, 
  std::string prefix, 
  Ptr<Ccnx> ccnx, 
  uint32_t face,
  bool explicitFilename)
{
  if (!m_ccnxEnabled)
    {
      NS_LOG_INFO ("Call to enable Ccnx ascii tracing but Ccnx not enabled");
      return;
    }

  //
  // Our trace sinks are going to use packet printing, so we have to 
  // make sure that is turned on.
  //
  Packet::EnablePrinting ();

  //
  // If we are not provided an OutputStreamWrapper, we are expected to create 
  // one using the usual trace filename conventions and hook WithoutContext
  // since there will be one file per context and therefore the context would
  // be redundant.
  //
  if (stream == 0)
    {
      //
      // Set up an output stream object to deal with private ofstream copy 
      // constructor and lifetime issues.  Let the helper decide the actual
      // name of the file given the prefix.
      //
      // We have to create a stream and a mapping from protocol/face to 
      // stream irrespective of how many times we want to trace a particular 
      // protocol.
      //
      AsciiTraceHelper asciiTraceHelper;

      std::string filename;
      if (explicitFilename)
        {
          filename = prefix;
        }
      else
        {
          filename = asciiTraceHelper.GetFilenameFromInterfacePair (prefix, ccnx, face);
        }

      Ptr<OutputStreamWrapper> theStream = asciiTraceHelper.CreateFileStream (filename);

      //
      // However, we only hook the trace sources once to avoid multiple trace sink
      // calls per event (connect is independent of face).
      //
      if (!AsciiHooked (ccnx))
        {
          //
          // The drop sink for the CcnxL3Protocol uses a different signature than
          // the default sink, so we have to cook one up for ourselves.  We can get
          // to the Ptr<CcnxL3Protocol> through our Ptr<Ccnx> since they must both 
          // be aggregated to the same node.
          //
          Ptr<CcnxL3Protocol> ccnxL3Protocol = ccnx->GetObject<CcnxL3Protocol> ();
          bool __attribute__ ((unused)) result = ccnxL3Protocol->TraceConnectWithoutContext ("Drop", 
                                                                                             MakeBoundCallback (&CcnxL3ProtocolDropSinkWithoutContext, theStream));
          NS_ASSERT_MSG (result == true, "CcnxStackHelper::EanableAsciiCcnxInternal():  "
                         "Unable to connect ccnxL3Protocol \"Drop\"");
        }

      g_faceStreamMapCcnx[std::make_pair (ccnx, face)] = theStream;
      return;
    }

  //
  // If we are provided an OutputStreamWrapper, we are expected to use it, and
  // to provide a context.  We are free to come up with our own context if we
  // want, and use the AsciiTraceHelper Hook*WithContext functions, but for 
  // compatibility and simplicity, we just use Config::Connect and let it deal
  // with the context.
  //
  // We need to associate the ccnx/face with a stream to express interest
  // in tracing events on that pair, however, we only hook the trace sources 
  // once to avoid multiple trace sink calls per event (connect is independent
  // of face).
  //
  if (!AsciiHooked (ccnx))
    {
      Ptr<Node> node = ccnx->GetObject<Node> ();
      std::ostringstream oss;

      //
      // This has all kinds of parameters coming with, so we have to cook up our
      // own sink.
      //
      oss.str ("");
      oss << "/NodeList/" << node->GetId () << "/$ns3::CcnxL3Protocol/Drop";
      Config::Connect (oss.str (), MakeBoundCallback (&CcnxL3ProtocolDropSinkWithContext, stream));
    }

  g_faceStreamMapCcnx[std::make_pair (ccnx, face)] = stream;
}

} // namespace ns3