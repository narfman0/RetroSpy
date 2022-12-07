using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Security;
using System.Text;
using System.Threading;

namespace RetroSpy
{
    public class PacketDataEventArgs : EventArgs
    {
        public PacketDataEventArgs(byte[] packet)
        {
            Packet = packet;
        }
        public byte[] GetPacket() { return Packet; }

        private readonly byte[] Packet;
    }

    //public delegate void PacketEventHandler(object sender, PacketDataEventArgs e);

    public class SerialMonitor : IDisposable
    {
        private const int BAUD_RATE = 115200;
        private const int TIMER_MS = 4;

        public event EventHandler<PacketDataEventArgs> PacketReceived;

        public event EventHandler Disconnected;

        private SerialPort _datPort;
        private readonly List<byte> _localBuffer;
        private Timer _timer;
        private readonly bool _printerMode;
        private static StreamWriter streamwriter;
        private bool ticking;
        private AutoResetEvent autoEvent;

        public SerialMonitor(string portName, bool useLagFix, bool printerMode = false)
        {
            _printerMode = printerMode;
            _localBuffer = new List<byte>();
            _datPort = new SerialPort(portName != null ? portName.Split(' ')[0] : "", useLagFix ? 57600 : BAUD_RATE);
        }

        public void Start()
        {
            if (_timer != null)
            {
                return;
            }

            _localBuffer.Clear();
            if (_printerMode)
            {
                _datPort.ReadBufferSize = 1000000;
            }

            _datPort.Open();

            streamwriter = null; //File.CreateText("./serialMonitorDebug.txt");
            autoEvent = new AutoResetEvent(false);
            _timer = new Timer(Tick, autoEvent, 0, TIMER_MS);
        }

        public void Stop()
        {
            if (_datPort != null)
            {
                try
                { // If the device has been unplugged, Close will throw an IOException.  This is fine, we'll just keep cleaning up.
                    _datPort.Close();
                }
                catch (IOException) { }
                _datPort.Dispose();
                _datPort = null;
            }
            if (_timer != null)
            {
                _timer.Dispose();
                _timer = null;
            }
            if(autoEvent != null)
            {
                autoEvent.Dispose();
                autoEvent = null;
            }
            if (streamwriter != null)
            {
                streamwriter.Flush();
                streamwriter.Close();
                streamwriter = null;
            }
        }

        private void Tick(Object stateInfo)
        {
            if (_datPort == null || !_datPort.IsOpen || PacketReceived == null || ticking)
            {
                return;
            }
            ticking = true;

            // Try to read some data from the COM port and append it to our localBuffer.
            // If there's an IOException then the device has been disconnected.
            try
            {
                int readCount = _datPort.BytesToRead;
                if (readCount < 1)
                {
                    if(streamwriter != null)
                        streamwriter.WriteLine(DateTimeOffset.Now.ToUnixTimeMilliseconds() + " readCount < 1");
                    ticking = false;
                    return;
                }

                byte[] readBuffer = new byte[readCount];
                _ = _datPort.Read(readBuffer, 0, readCount);
                //_datPort.DiscardInBuffer();
                _localBuffer.AddRange(readBuffer);
            }
            catch (IOException)
            {
                Stop();
                Disconnected?.Invoke(this, EventArgs.Empty);
                ticking = false;
                if (streamwriter != null)
                    streamwriter.WriteLine(DateTimeOffset.Now.ToUnixTimeMilliseconds() + " IOException");
                return;
            }

            // Try and find 2 splitting characters in our buffer.
            int lastSplitIndex = _localBuffer.LastIndexOf(0x0A);
            if (lastSplitIndex <= 1)
            {
                ticking = false;
                if (streamwriter != null)
                    streamwriter.WriteLine(DateTimeOffset.Now.ToUnixTimeMilliseconds() + " lastSplitIndex <= 1");
                return;
            }

            int sndLastSplitIndex = _localBuffer.LastIndexOf(0x0A, lastSplitIndex - 1);
            if (lastSplitIndex == -1)
            {
                ticking = false;
                if (streamwriter != null)
                    streamwriter.WriteLine(DateTimeOffset.Now.ToUnixTimeMilliseconds() + " lastSplitIndex == -1");
                return;
            }

            // Grab the latest packet out of the buffer and fire it off to the receive event listeners.
            int packetStart = sndLastSplitIndex + 1;
            int packetSize = lastSplitIndex - packetStart;

            if (_printerMode)
            {
                byte[] array = _localBuffer.ToArray();
                string lastCommand = Encoding.UTF8.GetString(array, 0, lastSplitIndex);

                if (lastCommand.Contains("# Finished Pretending To Print for fun!") || lastCommand.Contains("// Timed Out (Memory Waterline: 4B out of 400B)"))
                {
                    PacketReceived(this, new PacketDataEventArgs(_localBuffer.GetRange(0, lastSplitIndex).ToArray()));

                    // Clear our buffer up until the last split character.
                    _localBuffer.RemoveRange(0, lastSplitIndex);
                }
            }
            else
            {

                byte[] packet = _localBuffer.GetRange(packetStart, packetSize).ToArray();
                PacketReceived(this, new PacketDataEventArgs(packet));
                if(packet.Length > 2)
                {

                    var diff = packet[packet.Length - 2] + (packet[packet.Length - 1] << 8);
                    SendUdp(47569, "127.0.0.1", 47569, packet);
                    if (streamwriter != null)
                        streamwriter.WriteLine(DateTimeOffset.Now.ToUnixTimeMilliseconds() + " diff=" + diff);
                }

                // Clear our buffer up until the last split character.
                _localBuffer.RemoveRange(0, lastSplitIndex);
            }
            ticking = false;
        }
        private static void SendUdp(int srcPort, string dstIp, int dstPort, byte[] data)
        {
            using (UdpClient c = new UdpClient(srcPort))
                c.Send(data, data.Length, dstIp, dstPort);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                Stop();
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
    }
}