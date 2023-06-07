using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.IO;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace SM5002TestTool
{
    public partial class main : Form
    {
        public delegate void Delegate(object obj);
        public int serialDevice;
        public Thread scanThread = null;
        public bool bScanDeviceInfo = false;
        public int opDeviceCmd = 0;
        public int curGateIndex = 0;
        public main()
        {
            InitializeComponent();

            int hHFREADERDLLModule = 0;
            hHFREADERDLLModule = hfReaderDll.LoadLibrary("HFREADER.dll");

            if (hHFREADERDLLModule <= 0)
            {
                MessageBox.Show("Do not find hfreader.dll, Please check the file.");
                System.Environment.Exit(0);
            }

            this.Text += "    V" + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString();
        }
        private void UpdateComBox()
        {
            string[] portList = System.IO.Ports.SerialPort.GetPortNames();
            int i = 0;
            for (i = 0; i < comboBoxComPort.Items.Count; ++i)
            {
                int j = 0;
                string name = comboBoxComPort.Items[i].ToString();
                for (j = 0; j < portList.Length; j++)
                {
                    if (portList[j] == name)
                    {
                        break;
                    }
                }
                if (j == portList.Length)
                {
                    comboBoxComPort.Items.Remove(name);
                    comboBoxComPort.SelectedIndex = -1;
                }
            }
            for (i = 0; i < portList.Length; ++i)
            {
                string name = portList[i];
                if (comboBoxComPort.Items.IndexOf(name) < 0)
                {
                    comboBoxComPort.Items.Add(name);
                }
            }

            serialDevice = -1;
            comboBoxBaudrate.Text = "115200";
            comboBoxBaudrate.Enabled = false;
        }
        private void main_Load(object sender, EventArgs e)
        {
            comboBoxComPort.Items.Clear();
            UpdateComBox();
            int selectIndex = comboBoxComPort.Items.IndexOf(Settings.Default.com);
            if (selectIndex >= 0)
            {
                comboBoxComPort.SelectedIndex = selectIndex;
            }
            else
            {
                comboBoxComPort.SelectedIndex = -1;
            }
            comboBoxGateNo.SelectedIndex = 0;
            comboBoxFan1.SelectedIndex = 0;

            double[] k = { 97.8396, 92.3020, 87.1124, 82.2471, 77.6837, 73.4018, 69.3823, 65.6077, 62.0616, 58.7288,
                           55.5953, 52.6480, 49.8747, 47.2643, 44.8062, 42.4906, 40.3086, 38.2516, 36.3117, 34.4817,
                           32.7547, 31.1243, 29.5847, 28.1301, 26.7556, 25.4562, 24.2274, 23.0650, 21.9650, 20.9239,
                           19.9380, 19.0041, 18.1193, 17.2807, 16.4857, 15.7317, 15.0164, 14.3376, 13.6933, 13.0816,
                           12.5005, 11.9485, 11.4239, 10.9252, 10.4510, 10.0000, 9.5709, 9.1626, 8.7738, 8.4037,
                           8.0512, 7.7154, 7.3953, 7.0903, 6.7995, 6.5221, 6.2576, 6.0051, 5.7642, 5.5342,
                           5.3146, 5.1049, 4.9045, 4.7130, 4.5300, 4.3551, 4.1878, 4.0278, 3.8748, 3.7283,
                           3.5882, 3.4540, 3.3255, 3.2025, 3.0846, 2.9717, 2.8635, 2.7597, 2.6603, 2.5649,
                           2.4734, 2.3856, 2.3014, 2.2206, 2.1431, 2.0686, 1.9970, 1.9283, 1.8623, 1.7989,};

            string kStr = "";
            for (int i = 0; i < 90; i++)
            {
                double t = (1 * 50 * 4096) / (33 * (k[i] + 1));
                int tt = (int)t;
                kStr += tt.ToString() + ", ";
            }
            AddLog1Info(kStr);
        }

        public int GetHexInput(String s, Byte[] buffer, int num)
        {
            int i = 0;
            if (s.Length != 2 * num)
            {
                MessageBox.Show("数据长度错误");
                return -1;
            }
            for (i = 0; i < s.Length; i++)
            {
                char c = s[i];
                if ((c < '0' || c > '9') && (c < 'a' || c > 'f') && (c < 'A' || c > 'F'))
                {
                    MessageBox.Show("请以16进制格式输入数据，例如：00 01 FF");
                    return -1;
                }
            }
            for (i = 0; i < num; i++)
            {
                buffer[i] = Convert.ToByte(s.Substring(i * 2, 2), 16);
            }

            return num;
        }

        public bool GetDeviceAddr(ushort[] addArray)
        {
            bool b = false;
            byte[] buffer = new Byte[255];
            addArray[0] = 0x0000;
            if (GetHexInput(textBoxDestAddr.Text, buffer, 2) > 0)
            {
                addArray[1] = (ushort)(buffer[0] * 256 + buffer[1]);
                b = true;
            }

            return b;
        }

        private void GetDeviceVersion()
        {
            Byte[] buffer = new Byte[255];
            ushort[] addrArray = new ushort[2];

            if (!GetDeviceAddr(addrArray))
            {
                return;
            }

            HFREADER_VERSION pVersion = new HFREADER_VERSION();
            pVersion.type = new byte[hfReaderDll.HFREADER_VERSION_SIZE];
            pVersion.sv = new byte[hfReaderDll.HFREADER_VERSION_SIZE];
            pVersion.hv = new byte[hfReaderDll.HFREADER_VERSION_SIZE];
            int rlt = hfReaderDll.hfReaderGetVersion(serialDevice, 0x0000, addrArray[1], ref pVersion, null, null);
            if (rlt > 0)
            {
                if (pVersion.result.flag == 0)
                {
                    string deviceTypeStr = System.Text.Encoding.Default.GetString(pVersion.type).Replace("\0", "");
                    string sv = System.Text.Encoding.Default.GetString(pVersion.sv).Replace("\0", "");
                    string hv = System.Text.Encoding.Default.GetString(pVersion.hv).Replace("\0", "");

                    AddLog1Info("设备型号：" + deviceTypeStr + "  固件版本：" + sv + "  硬件版本：" + hv);
                    AddLog2Info("设备型号：" + deviceTypeStr + "  固件版本：" + sv + "  硬件版本：" + hv);
                }
            }
        }
        
        private void AddLog1Info(object obj)
        {
            if (this.textBoxLog1.InvokeRequired)
            {
                Delegate d = new Delegate(AddLog1Info);
                this.textBoxLog1.Invoke(d, obj);
            }
            else
            {
                if (obj == null)
                {
                    this.textBoxLog1.Text = "";
                }
                else
                {
                    this.textBoxLog1.Text += DateTime.Now.ToString("HH:mm:ss:ffff") + "\t" + obj.ToString() + "\r\n";
                }
                textBoxLog1.SelectionStart = textBoxLog1.Text.Length;
                textBoxLog1.ScrollToCaret();
            }
        }

        private void AddLog2Info(object obj)
        {
            if (this.textBoxLog2.InvokeRequired)
            {
                Delegate d = new Delegate(AddLog2Info);
                this.textBoxLog2.Invoke(d, obj);
            }
            else
            {
                if (obj == null)
                {
                    this.textBoxLog2.Text = "";
                }
                else
                {
                    this.textBoxLog2.Text += DateTime.Now.ToString("HH:mm:ss:ffff") + "\t" + obj.ToString() + "\r\n";
                }
                textBoxLog2.SelectionStart = textBoxLog2.Text.Length;
                textBoxLog2.ScrollToCaret();
            }
        }

        /*private void AddGateInfo(object obj)
        {
            if (this.groupBoxGate1.InvokeRequired)
            {
                Delegate d = new Delegate(AddGateInfo);
                this.groupBoxGate1.Invoke(d, obj);
            }
            else
            {
                if (obj == null)
                {
                    
                }
                else
                {
                    GATE_INFO g = (GATE_INFO)obj;
                    //AddLogInfo(null);
                    if (g.addr == "1")
                    {
                        strInfo = ("\r\n仓号：" + g.addr + "\r\n温度：" + g.tmpr + "℃\r\n" + g.sensorState + "\r\n" + g.batState + g.chagState);
                    }
                    else
                    {
                        AddLog2Info("\r\n仓号：" + g.addr + "\r\n温度：" + g.tmpr + "℃\r\n" + g.sensorState + "\r\n" + g.batState + g.chagState);
                    }
                }
            }*/
        

        public bool TranceviceFrame(Byte cmd, Byte[] txParamsBuf, int txParansLen, Byte[] rxParamsBuf, ref int rxParamsLen)
        {
            bool b = false;
            Byte[] txFrame = new Byte[4096];
            int txFrameLen = 0;
            Byte[] rxFrame = new Byte[4096];
            int rxFrameLen = 0;
            int repeatRx = 0;
            int repeatTx = 0;

            ushort[] addrArray = new ushort[2];

            if (!GetDeviceAddr(addrArray))
            {
                return false;
            }

            while (repeatTx < 2)
            {
                txFrameLen = hfReaderDll.hfReaderFormatFrame(0, addrArray[1], cmd, 0, 0, txParamsBuf, txParansLen, txFrame);
                if (txFrameLen > 0)
                {
                    while (repeatRx < 3)
                    {
                        hfReaderDll.hfReaderTransFrame(serialDevice, txFrame, (uint)txFrameLen);
                        rxFrameLen = hfReaderDll.hfReaderReceiveFrame(serialDevice, rxFrame, 2000);
                        repeatRx++;
                        if (rxFrameLen == 0 || (rxFrame[hfReaderDll.HFREADER_FRAME_POS_CMD + 1] != cmd))
                        {
                            Thread.Sleep(100);
                        }
                        else
                        {
                            break;
                        }
                    }
                    if (rxFrameLen > 0)
                    {
                        if (rxFrame[hfReaderDll.HFREADER_FRAME_POS_LEN] > 0)
                        {
                            rxParamsLen = rxFrame[hfReaderDll.HFREADER_FRAME_POS_LEN] + 3 - 14;
                            Array.Copy(rxFrame, hfReaderDll.HFREADER_FRAME_POS_PAR + 1, rxParamsBuf, 0, rxParamsLen);
                        }
                        else
                        {
                            rxParamsLen = rxFrame[hfReaderDll.HFREADER_FRAME_POS_PAR + 1] + rxFrame[hfReaderDll.HFREADER_FRAME_POS_PAR + 2] * 256;
                            Array.Copy(rxFrame, hfReaderDll.HFREADER_FRAME_POS_PAR + 3, rxParamsBuf, 0, rxParamsLen);
                        }

                        b = true;
                    }
                }
                if (b)
                {
                    break;
                }
                Thread.Sleep(100);
                repeatTx++;
            }


            return b;
        }

        public bool ReceiveFrame(Byte cmd, Byte[] rxParamsBuf, ref int rxParamsLen)
        {
            bool b = false;
            Byte[] rxFrame = new Byte[4096];
            int rxFrameLen = 0;

            ushort[] addrArray = new ushort[2];

            if (!GetDeviceAddr(addrArray))
            {
                return false;
            }

            rxFrameLen = hfReaderDll.hfReaderReceiveFrame(serialDevice, rxFrame, 20);
            if (rxFrame[hfReaderDll.HFREADER_FRAME_POS_CMD + 1] == cmd)
            {
                if (rxFrameLen > 0)
                {
                    if (rxFrame[hfReaderDll.HFREADER_FRAME_POS_LEN] > 0)
                    {
                        rxParamsLen = rxFrame[hfReaderDll.HFREADER_FRAME_POS_LEN] + 3 - 14;
                        Array.Copy(rxFrame, hfReaderDll.HFREADER_FRAME_POS_PAR + 1, rxParamsBuf, 0, rxParamsLen);
                    }
                    else
                    {
                        rxParamsLen = rxFrame[hfReaderDll.HFREADER_FRAME_POS_PAR + 1] + rxFrame[hfReaderDll.HFREADER_FRAME_POS_PAR + 2] * 256;
                        Array.Copy(rxFrame, hfReaderDll.HFREADER_FRAME_POS_PAR + 3, rxParamsBuf, 0, rxParamsLen);
                    }

                    b = true;
                }
            }
   
            return b;
        }

        private void GetSensorState(ref string sensorState, int flag)
        {
            if ((flag & 0x01) == 0x01)
            {
                sensorState += "Door-开启 ";
            }
            else
            {
                sensorState += "Door-关闭 ";
            }

            if ((flag & 0x02) == 0x02)
            {
                sensorState += "Rfid-开启 ";
            }
            else
            {
                sensorState += "Rfid-关闭 ";
            }

            if ((flag & 0x04) == 0x04)
            {
                sensorState += "烟感-报警 ";
            }
            else
            {
                sensorState += "烟感-正常 ";
            }

            if ((flag & 0x08) == 0x08)
            {
                sensorState += "风扇-开启";
            }
            else
            {
                sensorState += "风扇-关闭";
            }
        }

        private void GetBatState(ref string BatInfoStr, Byte[] rxGateInfo, ref int pos)
        {
            int t = 0, i = 0;

            t = rxGateInfo[pos++] + rxGateInfo[pos++] * 256;

            BatInfoStr = "电池信息:\r\n";
            if ((t & 0x01) == 0x01)
            {
                BatInfoStr += "\tBAT-存在";
            }
            else
            {
                BatInfoStr += "\tBAT-不存在\r\n";
                pos += 74;
                return;
            }

            if ((t & 0x80) == 0x80)
            {
                BatInfoStr += "-故障";
            }
            else
            {
                BatInfoStr += "-正常";
            }

            if ((t & 0x40) == 0x40)
            {
                BatInfoStr += "-充电满";
            }
            else
            {
                BatInfoStr += "-充电未满";
            }

            if ((t & 0x20) == 0x20)
            {
                BatInfoStr += "-充电中";
            }
            else
            {
                BatInfoStr += "-空闲";
            }
            BatInfoStr += "\r\n";

            BatInfoStr += "\t硬件版本-" + rxGateInfo[pos + 1].ToString() + "." + rxGateInfo[pos + 0].ToString() + "\r\n"; pos += 2;
            BatInfoStr += "\t固件版本-" + rxGateInfo[pos + 1].ToString() + "." + rxGateInfo[pos + 0].ToString() + "\r\n"; pos += 2;
            BatInfoStr += "\tSN-" + rxGateInfo[pos + 0].ToString("X").PadLeft(2, '0') + rxGateInfo[pos + 1].ToString("X").PadLeft(2, '0') +
                                    rxGateInfo[pos + 2].ToString("X").PadLeft(2, '0') + rxGateInfo[pos + 3].ToString("X").PadLeft(2, '0') +
                                    rxGateInfo[pos + 4].ToString("X").PadLeft(2, '0') + rxGateInfo[pos + 5].ToString("X").PadLeft(2, '0') + "\r\n";
            pos += 6;

            t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256; t /= 10;
            BatInfoStr += "\t电压-" + t.ToString() + "V";
            BatInfoStr += "\r\n";

            t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256;
            BatInfoStr += "\t电量-" + t.ToString() + "%";
            BatInfoStr += "\r\n";

            t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256;
            BatInfoStr += "\t周转-" + t.ToString() + "次";
            BatInfoStr += "\r\n";

            t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256;
            if (t == 0)
            {
                BatInfoStr += "\t充电状态-空闲";
            }
            else if (t == 1)
            {
                BatInfoStr += "\t充电状态-放电";
            }
            else if (t == 2)
            {
                BatInfoStr += "\t充电状态-充电";
            }
            BatInfoStr += "\r\n";

            t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256; t /= 10;
            BatInfoStr += "\t最大充电电压-" + t.ToString() + "V";
            BatInfoStr += "\r\n";

            t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256; t /= 10;
            BatInfoStr += "\t最大充电电流-" + t.ToString() + "A";
            BatInfoStr += "\r\n";

            t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256; t /= 100;
            BatInfoStr += "\t剩余容量-" + t.ToString() + "Ah";
            BatInfoStr += "\r\n";

            t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256; t /= 10;
            BatInfoStr += "\t充放电电流-" + t.ToString() + "A";
            BatInfoStr += "\r\n";

            t = rxGateInfo[pos++];
            BatInfoStr += "\t温度-" + t.ToString() + "℃";
            BatInfoStr += "\r\n";

            t = rxGateInfo[pos++];
            BatInfoStr += "\t单体压差-" + t.ToString() + "mV";
            BatInfoStr += "\r\n";

            BatInfoStr += "\t故障信息-" + rxGateInfo[pos++].ToString("X").PadLeft(2, '0') +
                                          rxGateInfo[pos++].ToString("X").PadLeft(2, '0') +
                                          rxGateInfo[pos++].ToString("X").PadLeft(2, '0') +
                                          rxGateInfo[pos++].ToString("X").PadLeft(2, '0') +
                                          rxGateInfo[pos++].ToString("X").PadLeft(2, '0');
            BatInfoStr += "\r\n";
            pos++;  //预留

            BatInfoStr += "\t单体电压00~03-";
            for (i = 0; i < 4; i++)
            {
                t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256;
                BatInfoStr += t.ToString() + "mV";
                if (i < 3)
                {
                    BatInfoStr += "  ";
                }
            }
            BatInfoStr += "\r\n";

            BatInfoStr += "\t单体电压04~07-";
            for (i = 0; i < 4; i++)
            {
                t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256;
                BatInfoStr += t.ToString() + "mV";
                if (i < 3)
                {
                    BatInfoStr += "  ";
                }
            }
            BatInfoStr += "\r\n";

            BatInfoStr += "\t单体电压08~11-";
            for (i = 0; i < 4; i++)
            {
                t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256;
                BatInfoStr += t.ToString() + "mV";
                if (i < 3)
                {
                    BatInfoStr += "  ";
                }
            }
            BatInfoStr += "\r\n";

            BatInfoStr += "\t单体电压12~15-";
            for (i = 0; i < 4; i++)
            {
                t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256;
                BatInfoStr += t.ToString() + "mV";
                if (i < 3)
                {
                    BatInfoStr += "  ";
                }
            }
            BatInfoStr += "\r\n";

            BatInfoStr += "\t单体电压16~19-";
            for (i = 0; i < 4; i++)
            {
                t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256;
                BatInfoStr += t.ToString() + "mV";
                if (i < 3)
                {
                    BatInfoStr += "  ";
                }
            }
            BatInfoStr += "\r\n";
        }

        private void GetChagState(ref string ChagInfoStr, Byte[] rxGateInfo, ref int pos)
        {
            int t = 0, i = 0;

            t = rxGateInfo[pos++];

            ChagInfoStr = "充电机信息:\r\n";
            if ((t & 0x01) == 0x01)
            {
                ChagInfoStr += "\t充电机-存在";
            }
            else
            {
                ChagInfoStr += "\t充电机-不存在";
                return;
            }

            if ((t & 0x80) == 0x80)
            {
                ChagInfoStr += "-故障";
            }
            else
            {
                ChagInfoStr += "-正常";
            }

            if ((t & 0x20) == 0x20)
            {
                ChagInfoStr += "-充电中";
            }
            else
            {
                ChagInfoStr += "-空闲";
            }
            ChagInfoStr += "\r\n";

            Byte[] typeName = new byte[4];
            Array.Copy(rxGateInfo, pos, typeName, 0, 4);

            System.Text.ASCIIEncoding enCode = new System.Text.ASCIIEncoding();
            ChagInfoStr += "\t厂家-" + enCode.GetString(typeName);
            pos += 5;
            ChagInfoStr += "\r\n";

            ChagInfoStr += "\t固件-" + rxGateInfo[pos + 0].ToString() + "." + rxGateInfo[pos + 1].ToString();
            pos += 2;
            ChagInfoStr += "\r\n";

            ChagInfoStr += "\t硬件-" + rxGateInfo[pos + 0].ToString() + "." + rxGateInfo[pos + 1].ToString();
            pos += 2;
            ChagInfoStr += "\r\n";

            t = rxGateInfo[pos++] + rxGateInfo[pos++] * 256;
            ChagInfoStr += "\t型号-" + t.ToString("X").PadLeft(4, '0');
            ChagInfoStr += "\r\n";

            t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256; t /= 10;
            ChagInfoStr += "\t功率-" + t.ToString() + "W";
            ChagInfoStr += "\r\n";

            t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256; t /= 100;
            ChagInfoStr += "\t最大充电电压-" + t.ToString() + "V";
            ChagInfoStr += "\r\n";

            t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256; t /= 100;
            ChagInfoStr += "\t最大充电电流-" + t.ToString() + "A";
            ChagInfoStr += "\r\n";

            t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256; t /= 10;
            ChagInfoStr += "\t当前充电电压-" + t.ToString() + "V";
            ChagInfoStr += "\r\n";

            t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256; //t /= 10;
            ChagInfoStr += "\t当前充电电流-" + t.ToString() + "mA";
            ChagInfoStr += "\r\n";

            t = rxGateInfo[pos++]; t += rxGateInfo[pos++] * 256; t /= 10;
            ChagInfoStr += "\t故障信息-" + t.ToString("X").PadLeft(4, '0');
            ChagInfoStr += "\r\n";
        }

        private int GetGateInfo(GATE_INFO[] gateInfo)
        {
            Byte[] gateInfoBuf = new Byte[4096];
            int gateInfoLen = 0;
            int pos = 0;
            int rcdNum = 0;
            if (TranceviceFrame(hfReaderDll.UART_FRAME_CMD_GET_ININFO, null, 0, gateInfoBuf, ref gateInfoLen))
            {
                rcdNum = gateInfoBuf[pos++];
                int i = 0;
                for (i = 0; i < rcdNum; i++)
                {
                    gateInfo[i].flag = (gateInfoBuf[pos + 0] + gateInfoBuf[pos + 1]).ToString("X").PadLeft(4, '0');
                    pos += 2;

                    gateInfo[i].addr = (gateInfoBuf[pos + 0]).ToString("X").PadLeft(2, '0');
                    pos += 1;

                    gateInfo[i].tmpr = (gateInfoBuf[pos + 0]).ToString("d");
                    pos += 1;

                    GetSensorState(ref gateInfo[i].sensorState, gateInfoBuf[pos + 0] + gateInfoBuf[pos + 1] * 256);
                    pos += 2;

                    GetBatState(ref gateInfo[i].batState, gateInfoBuf, ref pos);

                    GetChagState(ref gateInfo[i].chagState, gateInfoBuf, ref pos);

                    if (gateInfo[i].addr == "00")
                    {
                        AddLog1Info("\r\n仓号：" + gateInfo[i].addr +
                                    "\r\n温度：" + gateInfo[i].tmpr +
                                    "℃\r\n" + gateInfo[i].sensorState +
                                    "\r\n" + gateInfo[i].batState + gateInfo[i].chagState);
                    }
                    else
                    {
                        AddLog2Info("\r\n仓号：" + gateInfo[i].addr +
                                    "\r\n温度：" + gateInfo[i].tmpr +
                                    "℃\r\n" + gateInfo[i].sensorState +
                                    "\r\n" + gateInfo[i].batState + gateInfo[i].chagState);
                    }
                }
            } 

            return rcdNum;
        }

        private void GetBrwBatInfoStr(Byte[] paramsBuf, ref string strInfo)
        {
            if (paramsBuf[0] == 0x01)   //开门
            {
                if (paramsBuf[1] == 0x00)
                {
                    strInfo = ("\t开门成功\r\n");
                }
                else if (paramsBuf[1] == 0xFF)
                {
                    strInfo = ("\t等待开门\r\n");
                }
                else
                {
                    strInfo = ("\t未知错误\r\n");
                }
            }
            else if (paramsBuf[0] == 0x02)   //等待拿出电池和关门
            {
                if (paramsBuf[1] == 0x02)
                {
                    strInfo = ("\t仓门关闭，非法电池\r\n");
                }
                else if (paramsBuf[1] == 0x01)
                {
                    strInfo = ("\t等待关门，电池被取走\r\n");
                }
                else if (paramsBuf[1] == 0xFF)
                {
                    strInfo = ("\t等待关门，等待取走电池\r\n");
                }
                else
                {
                    strInfo = ("\t未知错误\r\n");
                }
            }
            else if (paramsBuf[0] == 0x00)   //结束
            {
                opDeviceCmd = 0;
                if (paramsBuf[1] == 0x03)
                {
                    strInfo = ("\t借电池结束：参数错误\r\n");
                }
                else if (paramsBuf[1] == 0x02)
                {
                    strInfo = ("\t借电池结束：电池未取走\r\n");
                }
                else if (paramsBuf[1] == 0x01)
                {
                    strInfo = ("\t借电池结束：开门失败\r\n");
                }
                else if (paramsBuf[1] == 0x00)
                {
                    strInfo = ("\t借电池结束：成功\r\n");
                }
                else
                {
                    strInfo = ("\t借电池结束：未知错误\r\n");
                }
            }
        }

        private void GetRtnBatInfoStr(Byte[] paramsBuf, ref string strInfo)
        {
            if (paramsBuf[0] == 0x01)   //开门
            {
                if (paramsBuf[1] == 0x00)
                {
                    strInfo = ("\t开门成功\r\n");
                }
                else if (paramsBuf[1] == 0xFF)
                {
                    strInfo = ("\t等待开门\r\n");
                }
                else
                {
                    strInfo = ("\t未知错误\r\n");
                }
            }
            else if (paramsBuf[0] == 0x02)   //等待拿出电池和关门
            {
                if (paramsBuf[1] == 0x04)
                {
                    strInfo = ("\t仓门关闭，非法电池\r\n");
                }
                else if (paramsBuf[1] == 0x02)
                {
                    strInfo = ("\t仓门关闭，电池未插入\r\n");
                }
                else if (paramsBuf[1] == 0x01)
                {
                    strInfo = ("\t等待关门，电池已插入\r\n");
                }
                else if (paramsBuf[1] == 0xFF)
                {
                    strInfo = ("\t等待关门，等待插入电池\r\n");
                }
                else
                {
                    strInfo = ("\t未知错误\r\n");
                }
            }
            else if (paramsBuf[0] == 0x00)   //结束
            {
                opDeviceCmd = 0;
                if (paramsBuf[1] == 0x03)
                {
                    strInfo = ("\t还电池结束：参数错误\r\n");
                }
                else if (paramsBuf[1] == 0x02)
                {
                    strInfo = ("\t还电池结束：电池未插入\r\n");
                }
                else if (paramsBuf[1] == 0x01)
                {
                    strInfo = ("\t还电池结束：开门失败\r\n");
                }
                else if (paramsBuf[1] == 0x00)
                {
                    strInfo = ("\t还电池结束：成功\r\n");
                }
                else
                {
                    strInfo = ("\t还电池结束：未知错误\r\n");
                }
            }
        }

        private bool ReceiveBowBatInfo()
        {
            bool bOver = false;
            Byte[] paramsBuf = new Byte[4096];
            int paramsLen = 0;
            int pos = 0;
            if (ReceiveFrame(hfReaderDll.UART_FRAME_CMD_BRWBAT, paramsBuf, ref paramsLen))
            {
                if (paramsLen == 2)
                {
                    string strInfo = "";

                    GetBrwBatInfoStr(paramsBuf, ref strInfo);
                    if (curGateIndex == 0)
                    {
                        AddLog1Info(strInfo);
                    }
                    else
                    {
                        AddLog2Info(strInfo);
                    }
                }
            }

            return bOver;
        }

        

        private bool ReceiveRtnBatInfo()
        {
            bool bOver = false;
            Byte[] paramsBuf = new Byte[4096];
            int paramsLen = 0;
            int pos = 0;
            if (ReceiveFrame(hfReaderDll.UART_FRAME_CMD_RTNBAT, paramsBuf, ref paramsLen))
            {
                if (paramsLen == 2)
                {
                    string strInfo = "";
                    GetRtnBatInfoStr(paramsBuf, ref strInfo);
                    if (curGateIndex == 0)
                    {
                        AddLog1Info(strInfo);
                    }
                    else
                    {
                        AddLog2Info(strInfo);
                    }
                }
            }

            return bOver;
        }

        private void buttonOpenSerial_Click(object sender, EventArgs e)
        {
            if (serialDevice <= 0)
            {
                string portName = this.comboBoxComPort.Text;
                if (portName.Length > 4)
                {
                    portName = "\\\\.\\" + portName;
                }
                serialDevice = hfReaderDll.hfReaderOpenPort(portName, this.comboBoxBaudrate.Text);
                if (serialDevice > 0)
                {
                    this.buttonOpenSerial.Text = "断开连接";
                    GetDeviceVersion();

                    Settings.Default.com = this.comboBoxComPort.Text;
                    Settings.Default.br = this.comboBoxBaudrate.Text;

                    Settings.Default.Save();

                    bScanDeviceInfo = true;
                    scanThread = new Thread(new ThreadStart(ScanGate));
                    scanThread.IsBackground = true;
                    scanThread.Start();
                }
                else
                {
                    MessageBox.Show("打开串口失败", "提示");
                }
            }
            else
            {
                bScanDeviceInfo = false;
                hfReaderDll.hfReaderClosePort(serialDevice);
                this.buttonOpenSerial.Text = "建立连接";
                serialDevice = -1;
            }
        }

        private void ScanGate()
        {
            while (bScanDeviceInfo)
            {
                Thread.Sleep(100);
                if (opDeviceCmd == hfReaderDll.UART_FRAME_CMD_BRWBAT)
                {
                    ReceiveBowBatInfo();
                }
                else if (opDeviceCmd == hfReaderDll.UART_FRAME_CMD_RTNBAT)
                {
                    ReceiveRtnBatInfo();
                }
                else
                {
                    GATE_INFO[] gateInfo = new GATE_INFO[10];
                    GetGateInfo(gateInfo);
                }
            }

            if (scanThread != null)
            {
                scanThread.Abort();
            }
            scanThread = null;
        }

        private void buttonClearLog_Click(object sender, EventArgs e)
        {
            AddLog1Info(null);
            AddLog2Info(null);
        }

        private void main_FormClosing(object sender, FormClosingEventArgs e)
        {
            bScanDeviceInfo = false;
        }

        private void buttonChargeStart1_Click(object sender, EventArgs e)
        {
            Byte[] txParamsBuf = new Byte[1024];
            int txParamsLen = 0;
            Byte[] rxParamsBuf = new Byte[1024];
            int rxParamsLen = 0;

            curGateIndex = comboBoxGateNo.SelectedIndex;
            txParamsBuf[txParamsLen++] = (Byte)comboBoxGateNo.SelectedIndex;
            txParamsBuf[txParamsLen++] = 1;
            if (TranceviceFrame(hfReaderDll.UART_FRAME_CMD_CHARGE, txParamsBuf, txParamsLen, rxParamsBuf, ref rxParamsLen))
            {
                MessageBox.Show("启动充电成功");
            }
            else
            {
                MessageBox.Show("启动充电失败");
            }
        }

        private void buttonChargeStop1_Click(object sender, EventArgs e)
        {
            Byte[] txParamsBuf = new Byte[1024];
            int txParamsLen = 0;
            Byte[] rxParamsBuf = new Byte[1024];
            int rxParamsLen = 0;
            txParamsBuf[txParamsLen++] = (Byte)comboBoxGateNo.SelectedIndex;
            txParamsBuf[txParamsLen++] = 0;
            if (TranceviceFrame(hfReaderDll.UART_FRAME_CMD_CHARGE, txParamsBuf, txParamsLen, rxParamsBuf, ref rxParamsLen))
            {
                MessageBox.Show("停止充电成功");
            }
            else
            {
                MessageBox.Show("停止充电失败");
            }
        }

        private bool Str2Hex(string str, Byte[] hex, int start, int num)
        {
            int i = 0;
            bool b = true;
            for (i = 0; i < num; i++)
            {
                try
                {
                    hex[start + i] = Convert.ToByte(str.Substring(i * 2, 2), 16);
                }
                catch (Exception ex)
                {
                    b = false;
                }
            }
            return b;
        }

        private void buttonBorrowBat1_Click(object sender, EventArgs e)
        {
            Byte[] txParamsBuf = new Byte[1024];
            int txParamsLen = 0;
            Byte[] rxParamsBuf = new Byte[1024];
            int rxParamsLen = 0;

            curGateIndex = comboBoxGateNo.SelectedIndex;
            txParamsBuf[txParamsLen++] = (Byte)comboBoxGateNo.SelectedIndex;
            Str2Hex(textBoxBat1Sn.Text, txParamsBuf, txParamsLen, 6);
            txParamsLen += 6;

            if (TranceviceFrame(hfReaderDll.UART_FRAME_CMD_BRWBAT, txParamsBuf, txParamsLen, rxParamsBuf, ref rxParamsLen))
            {
                opDeviceCmd = hfReaderDll.UART_FRAME_CMD_BRWBAT;
                string strInfo = "";
                GetBrwBatInfoStr(rxParamsBuf, ref strInfo);
                if (curGateIndex == 0)
                {
                    AddLog1Info(strInfo);
                }
                else
                {
                    AddLog2Info(strInfo);
                }
            }
            else
            {
                MessageBox.Show("启动借出电池失败");
            }
        }

        private void buttonReturnBat1_Click(object sender, EventArgs e)
        {
            Byte[] txParamsBuf = new Byte[1024];
            int txParamsLen = 0;
            Byte[] rxParamsBuf = new Byte[1024];
            int rxParamsLen = 0;

            txParamsBuf[txParamsLen++] = (Byte)comboBoxGateNo.SelectedIndex;
            Str2Hex(textBoxBat1Sn.Text, txParamsBuf, txParamsLen, 6);
            txParamsLen += 6;

            if (TranceviceFrame(hfReaderDll.UART_FRAME_CMD_RTNBAT, txParamsBuf, txParamsLen, rxParamsBuf, ref rxParamsLen))
            {
                opDeviceCmd = hfReaderDll.UART_FRAME_CMD_RTNBAT;
                string strInfo = "";
                GetRtnBatInfoStr(rxParamsBuf, ref strInfo);
                if (curGateIndex == 0)
                {
                    AddLog1Info(strInfo);
                }
                else
                {
                    AddLog2Info(strInfo);
                }
            }
            else
            {
                MessageBox.Show("启动借出电池失败");
            }
        }

        private void buttonCtrlOut_Click(object sender, EventArgs e)
        {
            Byte[] txParamsBuf = new Byte[1024];
            int txParamsLen = 0;
            Byte[] rxParamsBuf = new Byte[1024];
            int rxParamsLen = 0;

            txParamsBuf[0] = (Byte)comboBoxGateNo.SelectedIndex;
            if (checkBoxOpenDoor.Checked)
            {
                txParamsBuf[1] |= 0x02;
            }
            if (checkBoxFan.Checked)
            {
                txParamsBuf[1] |= 0x01;
            }
            txParamsLen = 2;

            if (TranceviceFrame(hfReaderDll.UART_FRAME_CMD_SET_OUTINFO, txParamsBuf, txParamsLen, rxParamsBuf, ref rxParamsLen))
            {
                MessageBox.Show("控制传感器成功");
            }
            else
            {
                MessageBox.Show("控制传感器失败");
            }
        }

        private void buttonSetParams_Click(object sender, EventArgs e)
        {
            Byte[] txParamsBuf = new Byte[1024];
            int txParamsLen = 0;
            Byte[] rxParamsBuf = new Byte[1024];
            int rxParamsLen = 0;
            int t = 0;

            txParamsBuf[txParamsLen++] = 0x00;      //RFU
            txParamsBuf[txParamsLen++] = 0x00;
            txParamsBuf[txParamsLen++] = (Byte)(((int)numericUpDownLedLowVol.Value >> 0) & 0xFF);
            txParamsBuf[txParamsLen++] = (Byte)(((int)numericUpDownLedLowVol.Value >> 8) & 0xFF);  

            txParamsBuf[txParamsLen++] = (Byte)(((int)numericUpDownFanStartTmpr.Value >> 0) & 0xFF);
            txParamsBuf[txParamsLen++] = (Byte)(((int)numericUpDownFanStartTmpr.Value >> 8) & 0xFF);

            txParamsBuf[txParamsLen++] = (Byte)(((int)numericUpDownBatFullLv.Value >> 0) & 0xFF);
            txParamsBuf[txParamsLen++] = (Byte)(((int)numericUpDownBatFullLv.Value >> 8) & 0xFF);

            txParamsBuf[txParamsLen++] = (Byte)(((int)numericUpDownBatLowLv.Value >> 0) & 0xFF);
            txParamsBuf[txParamsLen++] = (Byte)(((int)numericUpDownBatLowLv.Value >> 8) & 0xFF);

            txParamsBuf[txParamsLen++] = (Byte)(((int)numericUpDownBatHighLv.Value >> 0) & 0xFF);
            txParamsBuf[txParamsLen++] = (Byte)(((int)numericUpDownBatHighLv.Value >> 8) & 0xFF);

            t = Convert.ToInt32(textBoxChagLowVol.Text) * 10;
            txParamsBuf[txParamsLen++] = (Byte)((t >> 0) & 0xFF);
            txParamsBuf[txParamsLen++] = (Byte)((t >> 8) & 0xFF);

            t = Convert.ToInt32(textBoxChagLowCur.Text) * 100;
            txParamsBuf[txParamsLen++] = (Byte)((t >> 0) & 0xFF);
            txParamsBuf[txParamsLen++] = (Byte)((t >> 8) & 0xFF);

            t = Convert.ToInt32(textBoxChagHighVol.Text) * 10;
            txParamsBuf[txParamsLen++] = (Byte)((t >> 0) & 0xFF);
            txParamsBuf[txParamsLen++] = (Byte)((t >> 8) & 0xFF);

            t = Convert.ToInt32(textBoxChagHighCur.Text) * 100;
            txParamsBuf[txParamsLen++] = (Byte)((t >> 0) & 0xFF);
            txParamsBuf[txParamsLen++] = (Byte)((t >> 8) & 0xFF);

            t = Convert.ToInt32(textBoxChagFulVol.Text) * 10;
            txParamsBuf[txParamsLen++] = (Byte)((t >> 0) & 0xFF);
            txParamsBuf[txParamsLen++] = (Byte)((t >> 8) & 0xFF);

            t = Convert.ToInt32(textBoxChagFulCur.Text) * 100;
            txParamsBuf[txParamsLen++] = (Byte)((t >> 0) & 0xFF);
            txParamsBuf[txParamsLen++] = (Byte)((t >> 8) & 0xFF);

            if (TranceviceFrame(hfReaderDll.UART_FRAME_CMD_SET_PARAMS, txParamsBuf, txParamsLen, rxParamsBuf, ref rxParamsLen))
            {
                MessageBox.Show("设置参数成功");
            }
            else
            {
                MessageBox.Show("设置参数失败");
            }

        }

        private void buttonGetParams_Click(object sender, EventArgs e)
        {
            Byte[] txParamsBuf = new Byte[1024];
            int txParamsLen = 0;
            Byte[] rxParamsBuf = new Byte[1024];
            int rxParamsLen = 0;

            if (TranceviceFrame(hfReaderDll.UART_FRAME_CMD_GET_PARAMS, txParamsBuf, txParamsLen, rxParamsBuf, ref rxParamsLen))
            {
                int pos = 2;        //预留两个字节参数
                int t = 0;

                t = rxParamsBuf[pos + 0] + rxParamsBuf[pos + 1] * 256;  pos += 2;
                numericUpDownLedLowVol.Value = t;       //低电压指示电量

                t = rxParamsBuf[pos + 0] + rxParamsBuf[pos + 1] * 256; pos += 2;
                numericUpDownFanStartTmpr.Value = t;    //温度报警值

                t = rxParamsBuf[pos + 0] + rxParamsBuf[pos + 1] * 256; pos += 2;
                numericUpDownBatFullLv.Value = t;       //满电量值

                t = rxParamsBuf[pos + 0] + rxParamsBuf[pos + 1] * 256; pos += 2;
                numericUpDownBatLowLv.Value = t;        //低电量值

                t = rxParamsBuf[pos + 0] + rxParamsBuf[pos + 1] * 256; pos += 2;
                numericUpDownBatHighLv.Value = t;       //高电量值

                t = rxParamsBuf[pos + 0] + rxParamsBuf[pos + 1] * 256; pos += 2;
                t /= 10;
                textBoxChagLowVol.Text = t.ToString();
                t = rxParamsBuf[pos + 0] + rxParamsBuf[pos + 1] * 256; pos += 2;
                t /= 100;
                textBoxChagLowCur.Text = t.ToString();

                t = rxParamsBuf[pos + 0] + rxParamsBuf[pos + 1] * 256; pos += 2;
                t /= 10;
                textBoxChagHighVol.Text = t.ToString();
                t = rxParamsBuf[pos + 0] + rxParamsBuf[pos + 1] * 256; pos += 2;
                t /= 100;
                textBoxChagHighCur.Text = t.ToString();

                t = rxParamsBuf[pos + 0] + rxParamsBuf[pos + 1] * 256; pos += 2;
                t /= 10;
                textBoxChagFulVol.Text = t.ToString();
                t = rxParamsBuf[pos + 0] + rxParamsBuf[pos + 1] * 256; pos += 2;
                t /= 100;
                textBoxChagFulCur.Text = t.ToString();

                MessageBox.Show("获取参数成功");
            }
            else
            {
                MessageBox.Show("获取参数失败");
            }
        }

        private void buttonPlaneBat_Click(object sender, EventArgs e)
        {
            Byte[] txParamsBuf = new Byte[1024];
            int txParamsLen = 0;
            Byte[] rxParamsBuf = new Byte[1024];
            int rxParamsLen = 0;

            txParamsBuf[txParamsLen++] = (Byte)comboBoxGateNo.SelectedIndex;
            Str2Hex(textBoxBat1Sn.Text, txParamsBuf, txParamsLen, 6);
            txParamsLen += 6;
            txParamsBuf[txParamsLen++] = 0x01;

            if (TranceviceFrame(hfReaderDll.UART_FRAME_CMD_PLANE_BAT, txParamsBuf, txParamsLen, rxParamsBuf, ref rxParamsLen))
            {
                MessageBox.Show("预约电池成功");
            }
            else
            {
                MessageBox.Show("预约电池失败");
            }
        }

        private void buttonDelPlaneBat_Click(object sender, EventArgs e)
        {
            Byte[] txParamsBuf = new Byte[1024];
            int txParamsLen = 0;
            Byte[] rxParamsBuf = new Byte[1024];
            int rxParamsLen = 0;

            txParamsBuf[txParamsLen++] = (Byte)comboBoxGateNo.SelectedIndex;
            Str2Hex(textBoxBat1Sn.Text, txParamsBuf, txParamsLen, 6);
            txParamsLen += 6;
            txParamsBuf[txParamsLen++] = 0x00;

            if (TranceviceFrame(hfReaderDll.UART_FRAME_CMD_PLANE_BAT, txParamsBuf, txParamsLen, rxParamsBuf, ref rxParamsLen))
            {
                MessageBox.Show("取消预约电池成功");
            }
            else
            {
                MessageBox.Show("取消预约电池失败");
            }
        }
    }
}
