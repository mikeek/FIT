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

namespace Meal_Planner
{
    /// <summary>
    /// Interaction logic for CanvasRecipies.xaml
    /// </summary>
    /// 
    public partial class CanvasRecipies : UserControl
    {
        public CanvasRecipies()
        {
            InitializeComponent();
        }

        private void EditButton_Click(object sender, RoutedEventArgs e)
        {
            var selected = this.RecipiesListBox.SelectedIndex;
            this.RecipiesListBox.SelectedIndex = -1;
            this.RecipiesListBox.SelectedIndex = selected;
            this.RecipiesListBox.Items.Refresh();
        }

    }
}
