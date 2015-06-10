using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
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

namespace Meal_Planner.UserControls
{
    /// <summary>
    /// Interaction logic for FiveStarWidget.xaml
    /// </summary>
    public partial class FiveStarWidget : UserControl
    {
        public static DependencyProperty ValueProperty =
            DependencyProperty.Register("Value", typeof(byte?), typeof(FiveStarWidget),
              new PropertyMetadata(OnValuePropertyChanged));
        public byte? Value
        {
            get
            {
                return this.GetValue(ValueProperty) as byte?;
            }
            set
            {
                redrawStars();
                this.SetValue(ValueProperty, value);
            }
        }
        private static void OnValuePropertyChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
        {
            var c = sender as FiveStarWidget;
            if (c != null)
            {
                c.OnValueChanged();
            }
        }

        protected virtual void OnValueChanged()
        {
            // Grab related data.
            // Raises INotifyPropertyChanged.PropertyChanged
            NotifyPropertyChanged("Value");
        }

        protected override void OnPropertyChanged(DependencyPropertyChangedEventArgs e)
        {
            base.OnPropertyChanged(e);
            if (e.Property.Name == "Value")
            {
                redrawStars();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void NotifyPropertyChanged(String info)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(info));
            }
        }

        private List<Image> stars = new List<Image>();
        private List<Button> buttons = new List<Button>();

        public FiveStarWidget()
        {
            InitializeComponent();
            stars.Add(FiveStar.Star1);
            stars.Add(FiveStar.Star2);
            stars.Add(FiveStar.Star3);
            stars.Add(FiveStar.Star4);
            stars.Add(FiveStar.Star5);
            buttons.Add(FiveStar.Button1);
            buttons.Add(FiveStar.Button2);
            buttons.Add(FiveStar.Button3);
            buttons.Add(FiveStar.Button4);
            buttons.Add(FiveStar.Button5);
            for (int i = 0; i < buttons.Count; i++)
            {
                buttons[i].MouseEnter += Star_MouseChange;
                buttons[i].MouseLeave += Star_MouseChange;
                buttons[i].Click += Star_Click;
            }
            redrawStars();
        }

        private void Star_Click(object sender, RoutedEventArgs e)
        {
            if (!this.IsEnabled) return;

            Button clicked = sender as Button;
            int newVal = -1;
            for (int i = 0; i < buttons.Count; i++)
            {
                if (buttons[i] == clicked)
                    newVal = i + 1;
            }
            if (newVal == Value) {
                // Second click
                Value = null;
                return;
            }
            if (newVal != -1)
                Value = (byte)newVal;
        }

        private void Star_MouseChange(object sender, MouseEventArgs e)
        {
            redrawStars();
        }

        private void redrawStars()
        {
            var sourceNormal = findSource("star-normal.png");
            var sourceHoverNormal = findSource("star-hover-normal.png");
            var sourceFilled = findSource("star-filled.png");
            var sourceHoverFilled = findSource("star-hover-filled.png");

            int hover = -1;
            if (this.IsEnabled)
                for (int i = 0; i < stars.Count; i++)
                {
                    if (buttons[i].IsMouseOver)
                        hover = i;
                }

            for (int i = 0; i < stars.Count; i++)
            {
                if (this.IsEnabled)
                    stars[i].Cursor = Cursors.Hand;
                else
                    stars[i].Cursor = Cursors.Arrow;

                if (Value >= (i+1))
                    stars[i].Source = (hover >= i) ? sourceHoverFilled : sourceFilled;
                else
                    stars[i].Source = (hover >= i) ? sourceHoverNormal : sourceNormal;
            }
        }

        private BitmapImage findSource(string file)
        {
            return new BitmapImage(new Uri(@"pack://application:,,,/"
             + Assembly.GetExecutingAssembly().GetName().Name
             + ";component/"
             + "Resources/" + file, UriKind.Absolute)); 
        }
    }
}
