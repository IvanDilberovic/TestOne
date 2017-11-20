using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Web;
using System.Web.Mvc;

namespace TestOneWeb.Controllers
{
    public class HomeController : Controller
    {
        public ActionResult Index()
        {
            return View();
        }

        public ActionResult About()
        {
            ViewBag.Message = "Your application description page.";

            return View();
        }

        public JsonResult SaveImage(string slika)
        {
            var polje = slika.Split(',');

            var image = polje[1];

            var imageByte = Convert.FromBase64String(image);

            var stream = new MemoryStream(imageByte,0,imageByte.Length);

            var img = Image.FromStream(stream, true);
                      
            Bitmap resizedImage = ResizeImage(img,32,32);

            resizedImage = ClearAllPixels(resizedImage);

            resizedImage.Save(Server.MapPath("~/App_data/NacrtanaSlika.png"));

            stream = new MemoryStream();

            resizedImage.Save(stream, ImageFormat.Png);

            var resizedImageArray = stream.ToArray();

            var resizedImageBase64 = Convert.ToBase64String(resizedImageArray);

            var json = Json(polje[0] + "," + resizedImageBase64);

            return json;

        }

        private Bitmap ClearAllPixels(Bitmap resizedImage)
        {
            Bitmap newImage = new Bitmap(resizedImage.Width,resizedImage.Height);

            Color newColor = Color.White;

            for (int i = 0; i < resizedImage.Width; i++)
            {
                for (int j = 0; j < resizedImage.Height; j++)
                {
                    var pixel = resizedImage.GetPixel(i, j);

                    //System.Diagnostics.Debug.WriteLine($"{pixel.A} {pixel.R} {pixel.G} {pixel.B}");

                    if (pixel.A != 255)
                    {
                        newImage.SetPixel(i, j, newColor);
                    }
                    else
                    {
                        newImage.SetPixel(i,j,pixel);
                    }
                   

                }
            }

            return newImage;
        }

        public static Bitmap ResizeImage(Image image, int width, int height)
        {
            var destRect = new Rectangle(0, 0, width, height);
            var destImage = new Bitmap(width, height);

            destImage.SetResolution(image.HorizontalResolution, image.VerticalResolution);

            using (var graphics = Graphics.FromImage(destImage))
            {
                graphics.CompositingMode = CompositingMode.SourceCopy;
                graphics.CompositingQuality = CompositingQuality.HighQuality;
                graphics.InterpolationMode = InterpolationMode.Default;
                graphics.SmoothingMode = SmoothingMode.HighQuality;
                graphics.PixelOffsetMode = PixelOffsetMode.HighQuality;

                

                using (var wrapMode = new ImageAttributes())
                {
                    wrapMode.SetWrapMode(WrapMode.TileFlipXY);
                    graphics.DrawImage(image, destRect, 0, 0, image.Width, image.Height, GraphicsUnit.Pixel, wrapMode);
                }
            }

            return destImage;
        }
    }
}