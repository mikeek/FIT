using System;
using System.Collections.Generic;
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

namespace Meal_Planner.UserControls
{
    /// <summary>
    /// Interaction logic for LabelTextbox.xaml
    /// </summary>
    public partial class LabelTextbox : TextBox
    {
        private Brush OriginalBrush;
        private Brush OriginalBorder;

        // Pokud nechceme vykresleny ramecek, je potreba thickness nastavit
        // na cokoliv jineho, nez 1, protoze jinak se ramecek vykresli pri
        // mouse over efektu i kdyz jinak je jeho barva treba transparentni.
        // (Alespon ve W7 se tento bug projevi)
        private Thickness FakeThickness = new Thickness(1.0000001);

        public LabelTextbox()
        {
            InitializeComponent();
            OriginalBrush = Background;
            OriginalBorder = BorderBrush;
        }


        protected override void OnPropertyChanged(DependencyPropertyChangedEventArgs e)
        {
            base.OnPropertyChanged(e);
            if (e.Property.Name == "IsReadOnly")
            {
                if (IsReadOnly)
                {
                    OriginalBrush = Background;
                    OriginalBorder = BorderBrush;
                    BorderBrush = new SolidColorBrush(Colors.Transparent);
                    Background = new SolidColorBrush(Colors.Transparent);
                    if (BorderThickness.Bottom == 1 &&
                        BorderThickness.Left == 1 &&
                        BorderThickness.Right == 1
                        && BorderThickness.Top == 1)
                        BorderThickness = FakeThickness;
                    Cursor = Cursors.Arrow;
                }
                else
                {
                    BorderBrush = OriginalBorder;
                    Background = OriginalBrush;
                    if (BorderThickness == FakeThickness)
                        BorderThickness = new Thickness(1);
                    Cursor = Cursors.IBeam;
                }
            }
        }
    }
}
