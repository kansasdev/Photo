using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;
using Windows.System;
using WpfAboutView;

namespace WpfCamera
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private DispatcherTimer _timer;
        private BackgroundWorker _bw;
        private QAFace.FaceAnalysisEntity _fae;
        public MainWindow()
        {
            InitializeComponent();
            DeleteFaceFile(false);
            _timer = new DispatcherTimer(new TimeSpan(0, 0, 1), DispatcherPriority.Normal, new EventHandler(_timer_Elapsed), Dispatcher.CurrentDispatcher);
            _bw = new BackgroundWorker();
            _bw.DoWork += _bw_DoWork;
            _bw.RunWorkerCompleted += _bw_RunWorkerCompleted;

            _fae = new QAFace.FaceAnalysisEntity();
        }

        #region Backround worker methods
        private void _bw_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            btnImage.IsEnabled = true;
            btnQuality.IsEnabled = true;
            btnAbout.IsEnabled = true;
            pbWaitProgress.IsIndeterminate = false;
            if (e.Error == null)
            {
                string results = e.Result.ToString();
                if (!string.IsNullOrEmpty(results))
                {
                    string[] resSplitted = results.Split(new string[] { ";" }, StringSplitOptions.RemoveEmptyEntries);
                    tbFaces.Text = resSplitted[0];
                    tbMouthOpen.Text = resSplitted[1];
                    tbDynamicRange.Text = resSplitted[2];
                }
            }
            else
            {
                MessageBox.Show("Error: " + e.Error.Message);
            }

        }

        private void _bw_DoWork(object sender, DoWorkEventArgs e)
        {
            
            int res = _fae.CalculateQuality();
            if (res > 0)
            {
                e.Result = _fae.GetQualityResults();
            }
            else
            {
                throw new Exception("Error inside QAFace library");
            }
        }

        #endregion

        #region Button events
        private async void Button_Click(object sender, RoutedEventArgs e)
        {
            DeleteFaceFile(true);
            _timer.Stop();
            _timer.IsEnabled = false;
            var res = Launcher.LaunchUriAsync(new Uri("photo.photouwp:"));
            if (res.Status == Windows.Foundation.AsyncStatus.Started || res.Status==Windows.Foundation.AsyncStatus.Completed)
            {
                _timer.Start();
                _timer.IsEnabled = true;
            }
            else
            {
                MessageBox.Show("No PhotoUWP installed");
            }
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            if (openFileDialog.ShowDialog() == true)
            {
                
                byte[] b = File.ReadAllBytes(openFileDialog.FileName);
                DeleteFaceFile(false);
                if(!_timer.IsEnabled)
                {
                    _timer.Start();
                }
                File.WriteAllBytes(Environment.GetFolderPath(Environment.SpecialFolder.MyPictures) + "\\face.bmp", b);
                
            }
        }
        private void Button_About(object sender, RoutedEventArgs e)
        {
            new AboutDialog
            {
                Owner = this,
                AboutView = (AboutView)Resources["WpfCameraAboutView"]
            }.ShowDialog();

        }

        #endregion

        #region Functions
        private void DeleteFaceFile(bool doRemoveTextAndImage)
        {
            if (File.Exists(Environment.GetFolderPath(Environment.SpecialFolder.MyPictures) + "\\face.bmp"))
            {
                File.Delete(Environment.GetFolderPath(Environment.SpecialFolder.MyPictures) + "\\face.bmp");
            }
            if(doRemoveTextAndImage)
            {
                imageHost.Source = null;
                tbDynamicRange.Text = "";
                tbFaces.Text = "";
                tbMouthOpen.Text = "";
            }
        }


        private void _timer_Elapsed(object sender, System.EventArgs e)
        {
            if (File.Exists(Environment.GetFolderPath(Environment.SpecialFolder.MyPictures) + "\\face.bmp"))
            {
                _timer.Stop();
                _timer.IsEnabled = false;
                BitmapImage bi = new BitmapImage();
                bi.BeginInit();
                bi.CacheOption = BitmapCacheOption.OnLoad;
                bi.CreateOptions = BitmapCreateOptions.IgnoreImageCache;
                bi.UriSource = new Uri(Environment.GetFolderPath(Environment.SpecialFolder.MyPictures) + "\\face.bmp");
                bi.EndInit();
                imageHost.Source = bi;
                bi = null;

                btnImage.IsEnabled = false;
                btnQuality.IsEnabled = false;
                btnAbout.IsEnabled = false;
                pbWaitProgress.IsIndeterminate = true;

                _bw.RunWorkerAsync();

            }
        }
        #endregion

    }
}