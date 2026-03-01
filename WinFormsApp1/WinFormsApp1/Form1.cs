using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace WinFormsApp1
{
    public partial class Form1 : Form
    {
        private UdpClient udpServer;
        private Thread listenThread;
        private const int Port = 5000;

        private bool isEditingColor = false;
        private DateTime lastSendTime = DateTime.Now;

        // Параметры круга
        private float currentAngle = 0;
        private int radius = 100;
        private int thickness = 25;

        public Form1()
        {
            InitializeComponent();
            this.DoubleBuffered = true;

            labelStatus.Text = "Режим: ЯРКОСТЬ";
            trackBarValue.Value = 50;
            trackBarValue.Enabled = true; // По умолчанию яркость включена

            picColorWheel.Paint += PicColorWheel_Paint;
            picColorWheel.MouseDown += ColorWheel_Interaction;
            picColorWheel.MouseMove += ColorWheel_Interaction;
            trackBarValue.Scroll += trackBarValue_Scroll;
            CheckForIllegalCrossThreadCalls = false;




            StartListening();
        }




        private void ColorWheel_Interaction(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left && isEditingColor)
            {
                // 1. Центр строго по компоненту
                float cx = picColorWheel.Width / 2f;
                float cy = picColorWheel.Height / 2f;

                // 2. Считаем чистый угол (0 градусов — это "на 3 часа")
                double angle = Math.Atan2(e.Y - cy, e.X - cx) * (180 / Math.PI);

                // 3. Сдвигаем на 90 градусов, чтобы "0" визуально оказался наверху
                angle += -180;

                // 4. Нормализуем в диапазон 0-360
                if (angle < 0) angle += 360;
                if (angle >= 360) angle -= 360;

                // Присваиваем значение
                currentAngle = (float)angle;

                // Обновляем визуальные элементы
                if (panelCurrentColor != null)
                    panelCurrentColor.BackColor = ColorFromHue(currentAngle);

                picColorWheel.Invalidate();

                // 5. Лимит отправки пакетов
                if ((DateTime.Now - lastSendTime).TotalMilliseconds > 250)
                {
                    int valByte = (int)(currentAngle * 255 / 360);
                    SendCommand("$VAL:" + valByte);
                    lastSendTime = DateTime.Now;
                }
            }
        }

        private void trackBarValue_Scroll(object sender, EventArgs e)
        {
            // Ползунок работает только в режиме ЯРКОСТИ
            if (!isEditingColor)
            {
                if ((DateTime.Now - lastSendTime).TotalMilliseconds > 40)
                {
                    SendCommand("$VAL:" + trackBarValue.Value.ToString());
                    lastSendTime = DateTime.Now;
                }
            }
        }

        // --- ОТРИСОВКА ---

        private void PicColorWheel_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
            int cx = picColorWheel.Width / 2;
            int cy = picColorWheel.Height / 2;

            // Поворот холста: 0 градусов теперь сверху
            e.Graphics.TranslateTransform(cx, cy);
            e.Graphics.RotateTransform(-90);
            e.Graphics.TranslateTransform(-cx, -cy);

            if (isEditingColor)
            {
                // Активный режим: Радуга
                for (int i = 0; i < 360; i++)
                {
                    using (Pen p = new Pen(ColorFromHue(i), thickness))
                        e.Graphics.DrawArc(p, cx - radius, cy - radius, radius * 2, radius * 2, i, 2);
                }

                // Бегунок
                float rad = currentAngle * (float)(Math.PI / 180);
                float px = cx + (float)Math.Cos(rad) * radius;
                float py = cy + (float)Math.Sin(rad) * radius;
                e.Graphics.FillEllipse(Brushes.White, px - 12, py - 12, 24, 24);
                e.Graphics.DrawEllipse(Pens.Black, px - 12, py - 12, 24, 24);
            }
            else
            {
                // Неактивный режим: Серое кольцо
                using (Pen bg = new Pen(Color.DimGray, thickness))
                    e.Graphics.DrawEllipse(bg, cx - radius, cy - radius, radius * 2, radius * 2);
            }
        }

        // ТОТ САМЫЙ МЕТОД, КОТОРЫЙ ПРОПАЛ
        private Color ColorFromHue(double hue)
        {
            double h = hue / 60.0;
            double x = (1 - Math.Abs(h % 2.0 - 1.0));
            double r = 0, g = 0, b = 0;
            if (h < 1) { r = 1; g = x; }
            else if (h < 2) { r = x; g = 1; }
            else if (h < 3) { g = 1; b = x; }
            else if (h < 4) { g = x; b = 1; }
            else if (h < 5) { r = x; b = 1; }
            else { r = 1; b = x; }
            return Color.FromArgb((int)(r * 255), (int)(g * 255), (int)(b * 255));
        }

        // --- СЕТЕВАЯ ЧАСТЬ ---

        private void StartListening()
        {
            try
            {
                // Используем порт 5005 (на котором STM32 делает Broadcast)
                udpServer = new UdpClient(5005);
                listenThread = new Thread(() =>
                {
                    // Слушаем любые IP адреса
                    IPEndPoint remoteEP = new IPEndPoint(IPAddress.Any, 0);
                    while (true)
                    {
                        try
                        {
                            byte[] data = udpServer.Receive(ref remoteEP);
                            string message = Encoding.ASCII.GetString(data);

                            // 1. Поиск новых устройств (Маяки)
                            if (message.StartsWith("I_AM_STM32_IP:"))
                            {
                                string foundIp = message.Replace("I_AM_STM32_IP:", "").Trim();
                                this.Invoke(new MethodInvoker(() =>
                                {
                                    if (!listBoxDevices.Items.Contains(foundIp))
                                        listBoxDevices.Items.Add(foundIp);
                                }));
                            }

                            // 2. НОВОЕ: Обработка статуса по UDP (M:B|B:100|C:255)
                            else if (message.StartsWith("M:"))
                            {
                                // Вызываем обновление интерфейса напрямую из UDP потока
                                ParseAndUpdateStatus(message);
                            }
                        }
                        catch { /* Ошибки сокета при закрытии */ }
                    }
                });
                listenThread.IsBackground = true;
                listenThread.Start();
            }
            catch (Exception ex) { MessageBox.Show("Ошибка запуска UDP: " + ex.Message); }
        }

        private void ParseAndUpdateStatus(string status)
        {
            this.Invoke(new MethodInvoker(() =>
            {
                try
                {
                    // Разделяем строку M:B|B:100|C:255 по символу '|'
                    string[] parts = status.Split('|');
                    if (parts.Length < 3) return;

                    // 1. Парсим режим (M:B или M:C)
                    string mode = parts[0].Split(':')[1];
                    isEditingColor = (mode == "C");

                    // 2. Парсим яркость (B:100)
                    int brightness = int.Parse(parts[1].Split(':')[1]);

                    // 3. Парсим цвет (C:255)
                    int hue = int.Parse(parts[2].Split(':')[1]);

                    // Обновляем UI в зависимости от режима
                    labelStatus.Text = isEditingColor ? "РЕЖИМ: ЦВЕТ" : "РЕЖИМ: ЯРКОСТЬ";
                    trackBarValue.Enabled = !isEditingColor;


                    // Обновляем значения
                    if (!isEditingColor)
                    {
                        int newVal = Math.Clamp(brightness, trackBarValue.Minimum, trackBarValue.Maximum);
                        // Обновляем только если значение РЕАЛЬНО изменилось, чтобы не спамить Refresh
                        if (trackBarValue.Value != newVal)
                        {
                            trackBarValue.Value = newVal;
                        }
                    }

                    // Угол круга (0-255 -> 0-360)
                    currentAngle = (float)(hue * 360.0 / 255.0);

                    // Перерисовываем интерфейс
                    picColorWheel.Refresh();
                    if (panelCurrentColor != null)
                    {
                        panelCurrentColor.BackColor = ColorFromHue(currentAngle);
                    }
                }



                catch { /* Ошибка формата данных */ }
            }));
        }

        private void SendCommand(string command)
        {
            if (listBoxDevices.SelectedItem == null) return;
            string targetIp = listBoxDevices.SelectedItem.ToString();
            ThreadPool.QueueUserWorkItem(o =>
            {
                try
                {
                    using (TcpClient client = new TcpClient())
                    {
                        if (client.ConnectAsync(targetIp, 5000).Wait(500))
                        {
                            using (NetworkStream stream = client.GetStream())
                            {
                                byte[] data = Encoding.ASCII.GetBytes(command);
                                stream.Write(data, 0, data.Length);

                                byte[] buffer = new byte[32];
                                int read = stream.Read(buffer, 0, buffer.Length);
                                UpdateUI(Encoding.ASCII.GetString(buffer, 0, read).Trim());
                            }
                        }
                    }
                }
                catch { }
            });
        }



        private void button1_Click(object sender, EventArgs e) => SendCommand("$BTN");

        private void listBoxDevices_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listBoxDevices.SelectedItem != null)
                label1.Text = "Выбран IP: " + listBoxDevices.SelectedItem.ToString();
        }

        private void RequestCurrentStatus()
        {
            // Отправляем запрос только если выбрано устройство
            if (listBoxDevices.SelectedItem != null)
            {
                // Команда ?STAT скажет STM32, что мы хотим получить текущие параметры
                SendCommand("?STAT");
            }
        }

        private void UpdateUI(string response)
        {
            if (string.IsNullOrEmpty(response)) return;

            this.Invoke(new MethodInvoker(() =>
            {
                // Обработка расширенного статуса STAT:MODE=...;VAL=...
                if (response.StartsWith("STAT:"))
                {
                    // 1. Определяем режим
                    isEditingColor = response.Contains("MODE=COLOR");

                    // 2. Извлекаем значение (все что после VAL=)
                    try
                    {
                        string valStr = response.Substring(response.IndexOf("VAL=") + 4);
                        int value = int.Parse(valStr);

                        if (isEditingColor)
                        {
                            // Если сейчас цвет — обновляем угол круга (превращаем 0-255 обратно в 0-360)
                            currentAngle = (float)(value * 360 / 255);
                        }
                        else
                        {
                            // Если яркость — двигаем ползунок
                            if (value >= trackBarValue.Minimum && value <= trackBarValue.Maximum)
                                trackBarValue.Value = value;
                        }
                    }
                    catch { /* ошибка парсинга, игнорируем */ }

                    // Синхронизируем визуальную часть
                    labelStatus.Text = isEditingColor ? "РЕЖИМ: ЦВЕТ" : "РЕЖИМ: ЯРКОСТЬ";
                    trackBarValue.Enabled = !isEditingColor;
                    picColorWheel.Refresh();
                }

                // Старая логика для кнопки (если осталась)
                if (response.Contains("MODE_CHANGED"))
                {
                    isEditingColor = !isEditingColor;
                    trackBarValue.Enabled = !isEditingColor;
                    labelStatus.Text = isEditingColor ? "РЕЖИМ: ЦВЕТ" : "РЕЖИМ: ЯРКОСТЬ";
                    picColorWheel.Refresh();
                }
            }));
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            if (udpServer != null) udpServer.Close();
            if (listenThread != null) listenThread.Abort();
            base.OnFormClosing(e);
        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void picColorWheel_Click(object sender, EventArgs e)
        {

        }
    }
}